
/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "CellViewPage.h"

#include <QDebug>

#include <QMetaObject>
#include <QMetaMethod>
#include <QString>
#include <QSortFilterProxyModel>
#include <QPrintDialog>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QPrinter>
#include <QColorDialog>
#include <QImageReader>
#include <QPainter>
#include <QScrollArea>
#include <QRubberBand>
#include <QStyleFactory>

#include "error/Error.h"

#include "network/DownloadManager.h"

#include "CellViewPageToolBar.h"

#include "io/GeneExporter.h"

#include "utils/Utils.h"

#include "dialogs/SelectionDialog.h"
#include "dialogs/CreateSelectionDialog.h"

#include "viewOpenGL/CellGLView.h"
#include "viewOpenGL/ImageTextureGL.h"
#include "viewOpenGL/GridRendererGL.h"
#include "viewOpenGL/HeatMapLegendGL.h"
#include "viewOpenGL/MiniMapGL.h"
#include "viewOpenGL/GeneRendererGL.h"

#include "model/GeneSelectionItemModel.h"
#include "model/GeneFeatureItemModel.h"

#include "ui_cellview.h"

CellViewPage::CellViewPage(QWidget *parent)
    : Page(parent),
      m_minimap(nullptr),
      m_legend(nullptr),
      m_gene_plotter(nullptr),
      m_image(nullptr),
      m_grid(nullptr),
      m_view(nullptr),
      selectionDialog(nullptr),
      m_colorDialogGenes(nullptr),
      m_colorDialogGrid(nullptr)
{
    onInit();
}

CellViewPage::~CellViewPage()
{
    if (m_colorDialogGrid) {
        delete m_colorDialogGrid;
    }
    m_colorDialogGrid = nullptr;

    if (m_colorDialogGenes) {
        delete m_colorDialogGenes;
    }
    m_colorDialogGenes = nullptr;

    if (m_toolBar) {
        delete m_toolBar;
    }
    m_toolBar = nullptr;

    delete ui;
}

void CellViewPage::onInit()
{
    //create UI objects
    ui = new Ui::CellView;
    ui->setupUi(this);
    
    ui->clearSelection->setIcon(QIcon(QStringLiteral(":/images/clear2.png")));
    ui->saveSelection->setIcon(QIcon(QStringLiteral(":/images/file_export.png")));
    ui->exportSelection->setIcon(QIcon(QStringLiteral(":/images/export.png")));

    //gene search displays a clear button
    ui->lineEdit->setClearButtonEnabled(true);
    ui->geneSelectionFilterLineEdit->setClearButtonEnabled(true);

    // color dialogs
    m_colorDialogGrid = new QColorDialog(Globals::DEFAULT_COLOR_GRID);
     //OSX native color dialog gives problems
    m_colorDialogGrid->setOption(QColorDialog::DontUseNativeDialog, true);
    m_colorDialogGenes = new QColorDialog(Globals::DEFAULT_COLOR_GENE);
    //OSX native color dialog gives problems
    m_colorDialogGenes->setOption(QColorDialog::DontUseNativeDialog, true);

    // selection dialog
    selectionDialog = new SelectionDialog(this);

    //create tool bar and add it
    createToolBar();

    // init OpenGL graphical objects
    initGLView();

    //create connections
    createConnections();

    //create OpenGL connections
    createGLConnections();
}

void CellViewPage::onEnter()
{
    if (!loadData()) {
        //TODO show warning
        return;
    }

    DataProxy* dataProxy = DataProxy::getInstance();

    const auto dataset =
            dataProxy->getDatasetById(dataProxy->getSelectedDataset());
    Q_ASSERT(dataset);

    const auto imageAlignment =
            dataProxy->getImageAlignment(dataset->imageAlignmentId());
    Q_ASSERT(imageAlignment);

    const auto currentChip = dataProxy->getChip(imageAlignment->chipId());
    Q_ASSERT(currentChip);

    const QTransform alignment = imageAlignment->alignment();
    const qreal min = dataset->statisticsMin(); //1st quantile
    const qreal max = dataset->statisticsMax(); // 2nd quantile;
    const qreal pooledMin = dataset->statisticsPooledMin();
    const qreal pooledMax = dataset->statisticsPooledMax();

    const QRectF chip_rect = QRectF(
                QPointF(currentChip->x1(), currentChip->y1()),
                QPointF(currentChip->x2(), currentChip->y2())
                );
    const QRectF chip_border = QRectF(
                QPointF(currentChip->x1Border(), currentChip->y1Border()),
                QPointF(currentChip->x2Border(), currentChip->y2Border())
                );

    // updade grid size and data
    m_grid->clearData();
    m_grid->setDimensions(chip_border, chip_rect);
    m_grid->generateData();
    m_grid->setTransform(alignment);

    // update gene size an data
    m_gene_plotter->clearData();
    m_gene_plotter->setDimensions(chip_border);
    m_gene_plotter->generateData();
    m_gene_plotter->setTransform(alignment);
    m_gene_plotter->setHitCount(min, max, pooledMin, pooledMax);

    // updated legend size and data
    m_legend->setBoundaries(min, max);

    // load cell tissue
    slotLoadCellFigure();

    // reset main variabless
    resetActionStates();
}

void CellViewPage::onExit()
{
    ui->lineEdit->clearFocus();
    ui->geneSelectionFilterLineEdit->clearFocus();
    ui->genes_tableview->clearFocus();
    ui->showAllGenes->clearFocus();
    ui->hideAllGenes->clearFocus();
    ui->selections_tableview->clearFocus();
    ui->clearSelection->clearFocus();
    ui->saveSelection->clearFocus();
    ui->genes_tableview->clearSelection();
    ui->selections_tableview->clearSelection();

    m_gene_plotter->clearData();
    m_grid->clearData();
    //m_legend->clearData();
    //m_minimap->clearData();
    m_image->clear();
}

bool CellViewPage::loadData()
{
    DataProxy *dataProxy = DataProxy::getInstance();
    const auto dataset =
            dataProxy->getDatasetById(dataProxy->getSelectedDataset());

    if (dataset.isNull()) {
        showError("Cell View Error", "The current selected dataset is not valid.");
        return false;
    }

    setWaiting(true);

    //load the image alignment first
    {
        async::DataRequest request =
                dataProxy->loadImageAlignmentById(dataset->imageAlignmentId());
        if (request.return_code() == async::DataRequest::CodeError
                || request.return_code() == async::DataRequest::CodeAbort) {
            //TODO use text in request.getErrors()
            showError("Data loading Error", "Error loading the image alignment.");
            setWaiting(false);
            return false;
        }
    }

    //get image alignmet object
    const auto ImageAlignment =
            dataProxy->getImageAlignment(dataset->id());
    Q_ASSERT(ImageAlignment);

    //load cell tissue blue
    {
        async::DataRequest request =
                dataProxy->loadCellTissueByName(ImageAlignment->figureBlue());
        if (request.return_code() == async::DataRequest::CodeError
                || request.return_code() == async::DataRequest::CodeAbort) {
            //TODO use text in request.getErrors()
            showError("Data loading Error", "Error loading the cell tissue image(red).");
            setWaiting(false);
            return false;
        }
    }
    //load cell tissue red
    {
        async::DataRequest request =
                dataProxy->loadCellTissueByName(ImageAlignment->figureRed());
        if (request.return_code() == async::DataRequest::CodeError
                || request.return_code() == async::DataRequest::CodeAbort) {
            //TODO use text in request.getErrors()
            showError("Data loading Error", "Error loading the cell tissue image(blue).");
            setWaiting(false);
            return false;
        }
    }
    //load features
    {
        async::DataRequest request =
                dataProxy->loadFeatureByDatasetId(dataset->id());
        if (request.return_code() == async::DataRequest::CodeError
                || request.return_code() == async::DataRequest::CodeAbort) {
            //TODO use text in request.getErrors()
            showError("Data loading Error", "Error loading the features.");
            setWaiting(false);
            return false;
        }
    }
    //load genes
    {
        async::DataRequest request =
                dataProxy->loadGenesByDatasetId(dataset->id());
        if (request.return_code() == async::DataRequest::CodeError
                || request.return_code() == async::DataRequest::CodeAbort) {
            //TODO use text in request.getErrors()
            showError("Data loading Error", "Error loading the genes.");
            setWaiting(false);
            return false;
        }
    }
    //load chip
    {
        async::DataRequest request =
                dataProxy->loadChipById(ImageAlignment->chipId());
        if (request.return_code() == async::DataRequest::CodeError
                || request.return_code() == async::DataRequest::CodeAbort) {
            //TODO use text in request.getErrors()
            showError("Data loading Error", "Error loading the chip array.");
            setWaiting(false);
            return false;
        }
    }

    //succes downloading the data
    setWaiting(false);
    return true;
}

void CellViewPage::createConnections()
{
    // go back signal
    connect(m_toolBar->m_actionNavigate_goBack,
            SIGNAL(triggered(bool)), this, SIGNAL(moveToPreviousPage()));
    // go next signal
    connect(m_toolBar->m_actionNavigate_goNext,
            SIGNAL(triggered(bool)), this, SIGNAL(moveToNextPage()));

    // gene model signals
    connect(ui->lineEdit, SIGNAL(textChanged(QString)), geneProxyModel(),
            SLOT(setFilterFixedString(QString)));
    connect(ui->geneSelectionFilterLineEdit, SIGNAL(textChanged(QString)), selectionProxyModel(),
            SLOT(setFilterFixedString(QString)));
    connect(ui->showAllGenes, &QPushButton::clicked, this, [=](bool) {
            this->setVisibilityForAllGenes(true); });
    connect(ui->hideAllGenes, &QPushButton::clicked, this, [=](bool) {
            this->setVisibilityForAllGenes(false); });

    // cell tissue
    connect(m_toolBar->m_actionShow_cellTissueBlue,
            SIGNAL(triggered(bool)), this, SLOT(slotLoadCellFigure()));
    connect(m_toolBar->m_actionShow_cellTissueRed,
            SIGNAL(triggered(bool)), this, SLOT(slotLoadCellFigure()));

    // graphic view signals
    connect(m_toolBar->m_actionZoom_zoomIn,
            SIGNAL(triggered(bool)), m_view.data(), SLOT(zoomIn()));
    connect(m_toolBar->m_actionZoom_zoomOut,
            SIGNAL(triggered(bool)), m_view.data(), SLOT(zoomOut()));
    connect(m_toolBar, SIGNAL(rotateView(qreal)), m_view.data(), SLOT(rotate(qreal)));

    // print canvas
    connect(m_toolBar->m_actionSave_save,
            SIGNAL(triggered(bool)), this, SLOT(slotSaveImage()));
    connect(m_toolBar->m_actionSave_print,
            SIGNAL(triggered(bool)), this, SLOT(slotPrintImage()));

    // export selection
    connect(ui->exportSelection, SIGNAL(clicked(bool)),
            this, SLOT(slotExportSelection()));

    // save selection
    connect(ui->saveSelection, SIGNAL(clicked(bool)),
            this, SLOT(slotSaveSelection()));

    // selection mode
    connect(m_toolBar->m_actionActivateSelectionMode,
            SIGNAL(triggered(bool)), m_view.data(), SLOT(setSelectionMode(bool)));
    connect(m_toolBar->m_actionSelection_showSelectionDialog,
            SIGNAL(triggered(bool)), this, SLOT(slotSelectByRegExp()));

    //color selectors
    connect(m_toolBar->m_actionColor_selectColorGenes,
            SIGNAL(triggered(bool)), this, SLOT(slotLoadColor()));
    connect(m_toolBar->m_actionColor_selectColorGrid,
            SIGNAL(triggered(bool)), this, SLOT(slotLoadColor()));
}

QSortFilterProxyModel *CellViewPage::selectionProxyModel()
{
    QSortFilterProxyModel* selectionsProxyModel =
        qobject_cast<QSortFilterProxyModel*>(ui->selections_tableview->model());
    Q_ASSERT(selectionsProxyModel);
    return selectionsProxyModel;
}

GeneSelectionItemModel *CellViewPage::selectionModel()
{
    GeneSelectionItemModel* selectionModel =
        qobject_cast<GeneSelectionItemModel*>(selectionProxyModel()->sourceModel());
    Q_ASSERT(selectionModel);
    return selectionModel;
}

QSortFilterProxyModel *CellViewPage::geneProxyModel()
{
    QSortFilterProxyModel* proxyModel =
        qobject_cast<QSortFilterProxyModel*>(ui->genes_tableview->model());
    Q_ASSERT(proxyModel);
    return proxyModel;
}

GeneFeatureItemModel *CellViewPage::geneModel()
{
    GeneFeatureItemModel* geneModel =
        qobject_cast<GeneFeatureItemModel*>(geneProxyModel()->sourceModel());
    Q_ASSERT(geneModel);
    return geneModel;
}

void CellViewPage::resetActionStates()
{
    // reset gene model data
    geneModel()->loadGenes();

    // reset gene colors and selection
    //TODO fix this
    //geneModel()->setGeneColor(Globals::DEFAULT_COLOR_GENE);
    //geneModel()->setGeneVisibility(false);

    // reset gene selection model data
    selectionModel()->reset();

    // reset color dialogs
    m_colorDialogGenes->setCurrentColor(Globals::DEFAULT_COLOR_GENE);
    m_colorDialogGrid->setCurrentColor(Globals::DEFAULT_COLOR_GRID);

    // reset cell image to show
    m_image->setVisible(true);
    m_image->setAnchor(Globals::DEFAULT_ANCHOR_IMAGE);

    // reset gene grid to not show
    m_grid->setVisible(false);
    m_grid->setAnchor(Globals::DEFAULT_ANCHOR_GRID);

    // reset gene plotter to visible
    m_gene_plotter->setVisible(true);
    m_gene_plotter->setAnchor(Globals::DEFAULT_ANCHOR_GENE);

    // reset minimap to visible true
    m_minimap->setVisible(true);
    m_minimap->setAnchor(Globals::DEFAULT_ANCHOR_MINIMAP);

    // reset legend to visible true
    m_legend->setVisible(false);
    m_legend->setAnchor(Globals::DEFAULT_ANCHOR_LEGEND);

    // reset tool bar actions
    m_toolBar->resetActions();

    DataProxy *dataProxy = DataProxy::getInstance();
    // restrict interface
    DataProxy::UserPtr current_user = dataProxy->getUser();
    m_toolBar->m_actionGroup_cellTissue->setVisible(
                (current_user->role() == Globals::ROLE_CM));
}

void CellViewPage::createToolBar()
{
    m_toolBar = new CellViewPageToolBar();
    // add tool bar to the layout
    ui->pageLayout->insertWidget(0, m_toolBar);
}

void CellViewPage::initGLView()
{
    //ui->area contains the openGL window
    m_view = QSharedPointer<CellGLView>(new CellGLView());
    ui->area->initializeView(m_view);

    // Adding a stretch to make the opengl window occupy more space
    ui->selectionLayout->setStretch(1,10);

    // image texture graphical object
    m_image = QSharedPointer<ImageTextureGL>(new ImageTextureGL(this));
    m_image->setAnchor(Globals::DEFAULT_ANCHOR_IMAGE);
    m_view->addRenderingNode(m_image.data());

    // grid graphical object
    m_grid = QSharedPointer<GridRendererGL>(new GridRendererGL(this));
    m_grid->setAnchor(Globals::DEFAULT_ANCHOR_GRID);
    m_view->addRenderingNode(m_grid.data());

    // gene plotter component
    m_gene_plotter = QSharedPointer<GeneRendererGL>(new GeneRendererGL(this));
    m_gene_plotter->setAnchor(Globals::DEFAULT_ANCHOR_GENE);
    m_view->addRenderingNode(m_gene_plotter.data());

    // heatmap component
    m_legend = QSharedPointer<HeatMapLegendGL>(new HeatMapLegendGL(this));
    m_legend->setAnchor(Globals::DEFAULT_ANCHOR_LEGEND);
    m_view->addRenderingNode(m_legend.data());

    // minimap component
    m_minimap = QSharedPointer<MiniMapGL>(new MiniMapGL(this));
    m_minimap->setAnchor(Globals::DEFAULT_ANCHOR_MINIMAP);
    m_view->addRenderingNode(m_minimap.data());
    // minimap needs to be notified when the canvas is resized and when the image
    // is zoomed or moved
    connect(m_minimap.data(), SIGNAL(signalCenterOn(QPointF)),
            m_view.data(), SLOT(centerOn(QPointF)));
    connect(m_view.data(), SIGNAL(signalSceneUpdated(QRectF)),
            m_minimap.data(), SLOT(setScene(QRectF)));
    connect(m_view.data(), SIGNAL(signalViewPortUpdated(QRectF)),
            m_minimap.data(), SLOT(setViewPort(QRectF)));
    connect(m_view.data(), SIGNAL(signalSceneTransformationsUpdated(const QTransform)),
            m_minimap.data(), SLOT(setParentSceneTransformations(const QTransform)));
}

void CellViewPage::createGLConnections()
{
    //connect gene list model to gene plotter
    connect(geneModel(), SIGNAL(signalSelectionChanged(DataProxy::GeneList)),
            m_gene_plotter.data(),
            SLOT(updateSelection(DataProxy::GeneList)));
    connect(geneModel(), SIGNAL(signalColorChanged(DataProxy::GeneList)),
            m_gene_plotter.data(),
            SLOT(updateColor(DataProxy::GeneList)));

    //connect gene plotter to gene selection model
    connect(m_gene_plotter.data(), SIGNAL(selectionUpdated()),
            this, SLOT(slotSelectionUpdated()));

    // selection actions
    connect(ui->clearSelection, SIGNAL(clicked(bool)),
            m_gene_plotter.data(), SLOT(clearSelection()) );

    //threshold slider signal
    connect(m_toolBar, SIGNAL(thresholdLowerValueChanged(int)),
            m_gene_plotter.data(), SLOT(setLowerLimit(int)));
    connect(m_toolBar, SIGNAL(thresholdUpperValueChanged(int)),
            m_gene_plotter.data(), SLOT(setUpperLimit(int)));
    
    //gene attributes signals
    connect(m_toolBar, SIGNAL(intensityValueChanged(qreal)),
            m_gene_plotter.data(), SLOT(setIntensity(qreal)));
    connect(m_toolBar, SIGNAL(sizeValueChanged(qreal)),
            m_gene_plotter.data(), SLOT(setSize(qreal)));
    connect(m_toolBar, SIGNAL(shapeIndexChanged(Globals::GeneShape)),
            m_gene_plotter.data(), SLOT(setShape(Globals::GeneShape)));
    connect(m_toolBar, SIGNAL(shineValueChanged(qreal)),
            m_gene_plotter.data(), SLOT(setShine(qreal)));

    //show/not genes signal
    connect(m_toolBar->m_actionShow_showGenes,
            SIGNAL(triggered(bool)), m_gene_plotter.data(), SLOT(setVisible(bool)));

    //visual mode signal
    connect(m_toolBar->m_actionGroup_toggleVisualMode, SIGNAL(triggered(QAction*)), this,
            SLOT(slotSetGeneVisualMode(QAction*)));

    // grid signals
    connect(m_colorDialogGrid, SIGNAL(colorSelected(const QColor&)),
            m_grid.data(), SLOT(setColor(const QColor&)));
    connect(m_toolBar->m_actionShow_showGrid, SIGNAL(triggered(bool)),
            m_grid.data(), SLOT(setVisible(bool)));

    // cell tissue canvas
    connect(m_toolBar->m_actionShow_showCellTissue, SIGNAL(triggered(bool)),
            m_image.data(), SLOT(setVisible(bool)));
    connect(m_toolBar, SIGNAL(brightnessValueChanged(qreal)),
            m_image.data(), SLOT(setIntensity(qreal)));

    // legend signals
    connect(m_toolBar->m_actionShow_showLegend, SIGNAL(toggled(bool)),
            m_legend.data(), SLOT(setVisible(bool)));
    connect(m_toolBar->m_actionGroup_toggleLegendPosition,
            SIGNAL(triggered(QAction*)), this,
            SLOT(slotSetLegendAnchor(QAction*)));

    // minimap signals
    connect(m_toolBar->m_actionShow_showMiniMap, SIGNAL(toggled(bool)),
            m_minimap.data(), SLOT(setVisible(bool)));
    connect(m_toolBar->m_actionGroup_toggleMinimapPosition,
            SIGNAL(triggered(QAction*)), this,
            SLOT(slotSetMiniMapAnchor(QAction*)));

    // connect threshold slider to the heatmap
    connect(m_toolBar, SIGNAL(thresholdLowerValueChanged(int)),
            m_legend.data(), SLOT(setLowerLimit(int)));
    connect(m_toolBar, SIGNAL(thresholdUpperValueChanged(int)),
            m_legend.data(), SLOT(setUpperLimit(int)));
}

void CellViewPage::slotLoadCellFigure()
{
    DataProxy* dataProxy = DataProxy::getInstance();
    const auto current_user = dataProxy->getUser();
    Q_ASSERT(current_user);
    const auto dataset = dataProxy->getDatasetById(dataProxy->getSelectedDataset());
    Q_ASSERT(dataset);
    const auto imageAlignment = dataProxy->getImageAlignment(dataset->id());
    Q_ASSERT(imageAlignment);

    const bool forceRedFigure = QObject::sender() == m_toolBar->m_actionShow_cellTissueRed;
    const bool forceBlueFigure = QObject::sender() == m_toolBar->m_actionShow_cellTissueBlue;
    const bool defaultRedFigure = current_user->role() == Globals::ROLE_CM;
    const bool loadRedFigure = (defaultRedFigure || forceRedFigure) && !forceBlueFigure;

    QString figureid = (loadRedFigure) ? imageAlignment->figureRed() : imageAlignment->figureBlue();
    auto device = dataProxy->getFigure(figureid);

    //read image (TODO check file is present or corrupted)
    QImageReader reader(device.data());
    const QImage image = reader.read();

    //deallocate device
    device->deleteLater();

    // add image to the texture image holder
    m_image->createTexture(image);
    m_view->setScene(image.rect());

    //update checkboxes
    m_toolBar->m_actionShow_cellTissueBlue->setChecked(!loadRedFigure);
    m_toolBar->m_actionShow_cellTissueRed->setChecked(loadRedFigure);
}

void CellViewPage::slotPrintImage()
{
    QPrinter printer;
    printer.setOrientation(QPrinter::Landscape);
    QScopedPointer<QPrintDialog> dialog(new QPrintDialog(&printer, this));
    if (dialog->exec() != QDialog::Accepted) {
        return;
    }
    QPainter painter(&printer);
    QRect rect = painter.viewport();
    QImage image = m_view->grabPixmapGL();
    QSize size = image.size();
    size.scale(rect.size(), Qt::KeepAspectRatio);
    painter.setViewport(rect);
    painter.setWindow(image.rect());
    painter.drawImage(0, 0, image);
}

void CellViewPage::slotSaveImage()
{
    QString filename =
                       QFileDialog::getSaveFileName(this, tr("Save Image"), QDir::homePath(),
                       QString("%1;;%2").
                       arg(tr("JPEG Image Files (*.jpg *.jpeg)")).
                       arg(tr("PNG Image Files (*.png)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }
    // append default extension
    QRegExp regex("^.*\\.(jpg|jpeg|png)$", Qt::CaseInsensitive);
    if (!regex.exactMatch(filename)) {
        filename.append(".jpg");
    }
    const int quality = 100; //quality format (100 max, 0 min, -1 default)
    const QString format = filename.split(".", QString::SkipEmptyParts).at(1); //get the file extension
    QImage image = m_view->grabPixmapGL();
    if (!image.save(filename, format.toStdString().c_str(), quality)) {
        QMessageBox::warning(this, tr("Save Image"), tr("Error saving image."));
    }
}

void CellViewPage::slotExportSelection()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Export File"), QDir::homePath(),
                       QString("%1").
                       arg(tr("Text Files (*.txt)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }
    // append default extension
    QRegExp regex("^.*\\.(txt)$", Qt::CaseInsensitive);
    if (!regex.exactMatch(filename)) {
        filename.append(".txt");
    }

    // get selected features
    const auto& geneSelection = m_gene_plotter->getSelectedIItems();

    //create file
    QFile textFile(filename);

    //export selection
    if (textFile.open(QFile::WriteOnly | QFile::Truncate)) {
        GeneExporter exporter = GeneExporter(GeneExporter::SimpleFull,
                                             GeneExporter::TabDelimited);
        exporter.exportItem(&textFile, GeneSelection::getUniqueSelectedItems(geneSelection));
    }

    textFile.close();
}

void CellViewPage::slotSetGeneVisualMode(QAction *action)
{
    const QVariant variant = action->property("mode");
    if (variant.canConvert(QVariant::Int)) {
        const Globals::GeneVisualMode mode = static_cast<Globals::GeneVisualMode>(variant.toInt());
        m_gene_plotter->setVisualMode(mode);
    } else {
        Q_ASSERT("[CellViewPage] Undefined gene visual mode!");
    }
}

void CellViewPage::slotSetMiniMapAnchor(QAction *action)
{
    const QVariant variant = action->property("mode");
    if (variant.canConvert(QVariant::Int)) {
        const Globals::Anchor mode = static_cast<Globals::Anchor>(variant.toInt());
        m_minimap->setAnchor(mode);
    } else {
        Q_ASSERT("[CellViewPage] Undefined anchor!");
    }
}

void CellViewPage::slotSetLegendAnchor(QAction *action)
{
    const QVariant variant = action->property("mode");
    if (variant.canConvert(QVariant::Int)) {
        const Globals::Anchor mode = static_cast<Globals::Anchor>(variant.toInt());
        m_legend->setAnchor(mode);
    } else {
        Q_ASSERT("[CellViewPage] Undefined anchor!");
    }
}

void CellViewPage::slotLoadColor()
{
    if (QObject::sender() == m_toolBar->m_actionColor_selectColorGenes) {
        m_colorDialogGenes->open();
    } else if (QObject::sender() == m_toolBar->m_actionColor_selectColorGrid) {
        m_colorDialogGrid->open();
    }
}

void CellViewPage::slotSelectByRegExp()
{
    const DataProxy::GeneList& geneList = SelectionDialog::selectGenes(this);
    m_gene_plotter->selectGenes(geneList);
}

void CellViewPage::slotSelectionUpdated()
{
    // get selected features
    const auto& geneSelection = m_gene_plotter->getSelectedIItems();
    selectionModel()->loadSelectedGenes(GeneSelection::getUniqueSelectedItems(geneSelection));
}

void CellViewPage::slotSaveSelection()
{
    QScopedPointer<CreateSelectionDialog> createSelection(new CreateSelectionDialog(this,
                                                                          Qt::CustomizeWindowHint | Qt::WindowTitleHint));
    if (createSelection->exec() == CreateSelectionDialog::Accepted) {
        DataProxy *dataProxy = DataProxy::getInstance();
        // get selected features
        const auto& geneSelection = m_gene_plotter->getSelectedIItems();
        //create the selection object
        GeneSelection selection;
        selection.name(createSelection->getName());
        selection.comment(createSelection->getComment());
        selection.type("Bounding box");
        DataProxy::DatasetPtr dataset = dataProxy->getDatasetById(dataProxy->getSelectedDataset());
        Q_ASSERT(dataset != nullptr);
        selection.datasetId(dataset->id());
        selection.selectedItems(geneSelection);
        //save the selection object
        async::DataRequest request = dataProxy->addGeneSelection(selection);
        if (request.return_code() == async::DataRequest::CodeError
                || request.return_code() == async::DataRequest::CodeAbort) {
            //TODO use the test in request.getErrors()
            showError("Create Gene Selection", "Error saving the gene selection");
        }
        else {
            qDebug() << "Selection object saved succesfully";
        }

    }
}

void CellViewPage::setVisibilityForAllGenes(bool visible)
{
    const int rows = geneProxyModel()->rowCount(QModelIndex());
    if (rows > 0) {
        QModelIndex topLeft = geneProxyModel()->index(0, 0, QModelIndex());
        QModelIndex bottomLeft = geneProxyModel()->index(rows-1, 0, QModelIndex());
        QItemSelection proxySelection(topLeft, bottomLeft);
        QItemSelection selection = geneProxyModel()->mapSelectionToSource(proxySelection);
        geneModel()->setGeneVisibility(selection, visible);
    }
}
