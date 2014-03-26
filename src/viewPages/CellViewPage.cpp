
/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "CellViewPage.h"

#include <QDebug>
#include "utils/DebugHelper.h"

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

#include "CellViewPageToolBar.h"

#include "io/GeneXMLExporter.h"
#include "io/GeneTXTExporter.h"

#include "utils/Utils.h"
#include "dialogs/SelectionDialog.h"

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
    : Page(parent)
{
    onInit();
}

CellViewPage::~CellViewPage()
{
    if (m_colorDialogGrid) {
        delete m_colorDialogGrid;
    }
    m_colorDialogGrid = 0;

    if (m_colorDialogGenes) {
        delete m_colorDialogGenes;
    }
    m_colorDialogGenes = 0;

    if (m_toolBar) {
        delete m_toolBar;
    }
    m_toolBar = 0;

    // grid gene_plotter and image are removed automatically

    delete ui;
}

void CellViewPage::onInit()
{
    DEBUG_FUNC_NAME
    
    //create UI objects
    ui = new Ui::CellView;
    ui->setupUi(this);
    
    ui->clearSelection->setIcon(QIcon(QStringLiteral(":/images/clear2.png")));
    ui->saveSelection->setIcon(QIcon(QStringLiteral(":/images/file_export.png")));
    ui->exportSelection->setIcon(QIcon(QStringLiteral(":/images/export.png")));

    //gene search displays a clear button
    ui->lineEdit->setClearButtonEnabled(true);

    //mouse tracking could cause problems on OSX
#if defined Q_OS_MAC
    ui->lineEdit->setMouseTracking(true);
    ui->lineEdit->setFocusPolicy(Qt::ClickFocus);
    ui->genes_tableview->setFocusPolicy(Qt::NoFocus);
    ui->genes_tableview->setMouseTracking(false);
    ui->selectAllGenes->setFocusPolicy(Qt::NoFocus);
    ui->selectAllGenes->setMouseTracking(false);
#endif

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
    DEBUG_FUNC_NAME

    DataProxy* dataProxy = DataProxy::getInstance();

    DataProxy::DatasetPtr dataset =
            dataProxy->getDatasetById(dataProxy->getSelectedDataset());
    Q_ASSERT(dataset);

    DataProxy::ChipPtr currentChip = dataProxy->getChip(dataset->chipId());
    Q_ASSERT(currentChip);

    DataProxy::DatasetStatisticsPtr statistics =
            dataProxy->getStatistics(dataProxy->getSelectedDataset());
    Q_ASSERT(statistics);

    const QTransform alignment = dataset->alignment();

    const qreal min = statistics->min(); //1st quantile
    const qreal max = statistics->max(); // 2nd quantile;
    const qreal pooledMin = statistics->pooledMin();
    const qreal pooledMax = statistics->pooledMax();
    //const qreal sum = statistics->sum(); // total sum

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
    DEBUG_FUNC_NAME

    ui->lineEdit->clearFocus();
    ui->genes_tableview->clearFocus();
    ui->selectAllGenes->clearFocus();
    ui->selections_tableview->clearFocus();
    ui->clearSelection->clearFocus();
    ui->saveSelection->clearFocus();
}

void CellViewPage::createConnections()
{
    DEBUG_FUNC_NAME

    // go back signal
    connect(m_toolBar->m_actionNavigate_goBack,
            SIGNAL(triggered(bool)), this, SIGNAL(moveToPreviousPage()));

    // gene model signals
    QSortFilterProxyModel *proxyModel =
            qobject_cast<QSortFilterProxyModel*>(ui->genes_tableview->model());
    GeneFeatureItemModel *geneModel =
            qobject_cast<GeneFeatureItemModel*>(proxyModel->sourceModel());
    connect(ui->lineEdit, SIGNAL(textChanged(QString)), proxyModel,
            SLOT(setFilterFixedString(QString)));
    connect(ui->selectAllGenes, SIGNAL(clicked(bool)), geneModel,
            SLOT(selectAllGenesPressed(bool)));
    connect(m_colorDialogGenes, SIGNAL(colorSelected(QColor)), geneModel,
            SLOT(setColorGenes(const QColor&)));

    // cell tissue
    connect(m_toolBar->m_actionShow_cellTissueBlue,
            SIGNAL(triggered(bool)), this, SLOT(slotLoadCellFigure()));
    connect(m_toolBar->m_actionShow_cellTissueRed,
            SIGNAL(triggered(bool)), this, SLOT(slotLoadCellFigure()));

    // graphic view signals
    connect(m_toolBar->m_actionZoom_zoomIn,
            SIGNAL(triggered(bool)), m_view, SLOT(zoomIn()));
    connect(m_toolBar->m_actionZoom_zoomOut,
            SIGNAL(triggered(bool)), m_view, SLOT(zoomOut()));
    connect(m_toolBar, SIGNAL(rotateView(qreal)), m_view, SLOT(rotate(qreal)));

    // print canvas
    connect(m_toolBar->m_actionSave_save,  SIGNAL(triggered(bool)), this, SLOT(slotSaveImage()));
    connect(m_toolBar->m_actionSave_print, SIGNAL(triggered(bool)), this, SLOT(slotPrintImage()));

    // export
    connect(ui->exportSelection, SIGNAL(clicked(bool)), this, SLOT(slotExportSelection()));

    // selection mode
    connect(m_toolBar->m_actionSelection_showSelectionDialog,
            SIGNAL(triggered(bool)), this, SLOT(slotSelectByRegExp()));

    //color selectors
    connect(m_toolBar->m_actionColor_selectColorGenes,
            SIGNAL(triggered(bool)), this, SLOT(slotLoadColor()));
    connect(m_toolBar->m_actionColor_selectColorGrid,
            SIGNAL(triggered(bool)), this, SLOT(slotLoadColor()));
}

void CellViewPage::resetActionStates()
{
    // load gene model (need to be done first)
    QSortFilterProxyModel* proxyModel =
            qobject_cast<QSortFilterProxyModel*>(ui->genes_tableview->model());
    GeneFeatureItemModel* geneModel =
            qobject_cast<GeneFeatureItemModel*>(proxyModel->sourceModel());
    GeneSelectionItemModel* selectionModel =
            qobject_cast<GeneSelectionItemModel*>(ui->selections_tableview->model());

    // reset gene model data
    geneModel->loadGenes();

    // reset gene colors
    geneModel->setColorGenes(Globals::DEFAULT_COLOR_GENE);

    // set all genes selected to false
    geneModel->selectAllGenesPressed(false);
    
    // reset gene selection model data
    selectionModel->reset();

    // reset color dialogs
    m_colorDialogGenes->setCurrentColor(Globals::DEFAULT_COLOR_GENE);
    m_colorDialogGrid->setCurrentColor(Globals::DEFAULT_COLOR_GRID);

    // reset gene list selection status
    ui->selectAllGenes->setChecked(false);

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
    Q_ASSERT(current_user);
    m_toolBar->m_actionGroup_cellTissue->setVisible((current_user->role() == Globals::ROLE_CM));
}

void CellViewPage::createToolBar()
{
    m_toolBar = new CellViewPageToolBar();
    // add tool bar to the layout
    ui->pageLayout->insertWidget(0, m_toolBar);
}

void CellViewPage::initGLView()
{
    DEBUG_FUNC_NAME

    // creates graphic canvas scene and view
    m_view = new CellGLView();
    QWidget *container = QWidget::createWindowContainer(m_view);
    container->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ui->mainLayout->addWidget(container);
    // Adding a stretch to make the opengl window occupy more space
    ui->mainLayout->setStretch(1,10);

    // image texture graphical object
    m_image = new ImageTextureGL(this);
    m_image->setAnchor(Globals::DEFAULT_ANCHOR_IMAGE);
    m_view->addRenderingNode(m_image);

    // grid graphical object
    m_grid = new GridRendererGL(this);
    m_grid->setAnchor(Globals::DEFAULT_ANCHOR_GRID);
    m_view->addRenderingNode(m_grid);

    // gene plotter component
    m_gene_plotter = new GeneRendererGL(this);
    m_gene_plotter->setAnchor(Globals::DEFAULT_ANCHOR_GENE);
    m_view->addRenderingNode(m_gene_plotter);

    // heatmap component
    m_legend = new HeatMapLegendGL(this);
    m_legend->setAnchor(Globals::DEFAULT_ANCHOR_LEGEND);
    m_view->addRenderingNode(m_legend);

    // minimap component
    m_minimap = new MiniMapGL(this);
    m_minimap->setAnchor(Globals::DEFAULT_ANCHOR_MINIMAP);
    m_view->addRenderingNode(m_minimap);
    // minimap needs to be notified when the canvas is resized and when the image
    // is zoomed or moved
    connect(m_minimap, SIGNAL(signalCenterOn(QPointF)), m_view, SLOT(centerOn(QPointF)));
    connect(m_view, SIGNAL(signalSceneUpdated(QRectF)), m_minimap, SLOT(setScene(QRectF)));
    connect(m_view, SIGNAL(signalViewPortUpdated(QRectF)), m_minimap, SLOT(setViewPort(QRectF)));
    connect(m_view, SIGNAL(signalSceneTransformationsUpdated(const QTransform)), m_minimap, SLOT(setParentSceneTransformations(const QTransform)));
}

void CellViewPage::createGLConnections()
{
    // gene model signals
    QSortFilterProxyModel* proxyModel =
            qobject_cast<QSortFilterProxyModel*>(ui->genes_tableview->model());
    GeneFeatureItemModel* geneModel =
            qobject_cast<GeneFeatureItemModel*>(proxyModel->sourceModel());

    //connect gene list model to gene plotter
    connect(geneModel, SIGNAL(signalSelectionChanged(DataProxy::GenePtr)), m_gene_plotter,
            SLOT(updateSelection(DataProxy::GenePtr)));
    connect(geneModel, SIGNAL(signalColorChanged(DataProxy::GenePtr)), m_gene_plotter,
            SLOT(updateColor(DataProxy::GenePtr)));
    connect(ui->selectAllGenes, SIGNAL(clicked(bool)), m_gene_plotter, SLOT(updateAllSelection(bool)));
    connect(m_colorDialogGenes, SIGNAL(colorSelected(QColor)), m_gene_plotter, SLOT(updateAllColor(QColor)));

    //connect gene plotter to gene selection model
    connect(m_gene_plotter, SIGNAL(selectionUpdated()), this, SLOT(selectionUpdated()));

    // selection actions
    connect(ui->clearSelection, SIGNAL(clicked(bool)), m_gene_plotter, SLOT(clearSelection()) );

    //threshold slider signal
    connect(m_toolBar, SIGNAL(thresholdLowerValueChanged(int)), m_gene_plotter, SLOT(setLowerLimit(int)));
    connect(m_toolBar, SIGNAL(thresholdUpperValueChanged(int)), m_gene_plotter, SLOT(setUpperLimit(int)));
    
    //gene attributes signals
    connect(m_toolBar, SIGNAL(intensityValueChanged(qreal)), m_gene_plotter, SLOT(setIntensity(qreal)));
    connect(m_toolBar, SIGNAL(sizeValueChanged(qreal)), m_gene_plotter, SLOT(setSize(qreal)));
    connect(m_toolBar, SIGNAL(shapeIndexChanged(Globals::GeneShape)),
            m_gene_plotter, SLOT(setShape(Globals::GeneShape)));
    connect(m_toolBar, SIGNAL(shineValueChanged(qreal)), m_gene_plotter, SLOT(setShine(qreal)));

    //show/not genes signal
    connect(m_toolBar->m_actionShow_showGenes, SIGNAL(triggered(bool)), m_gene_plotter, SLOT(setVisible(bool)));

    //visual mode signal
    connect(m_toolBar->m_actionGroup_toggleVisualMode, SIGNAL(triggered(QAction*)), this,
            SLOT(slotSetGeneVisualMode(QAction*)));

    // grid signals
    connect(m_colorDialogGrid, SIGNAL(colorSelected(const QColor&)), m_grid, SLOT(setColor(const QColor&)));
    connect(m_toolBar->m_actionShow_showGrid, SIGNAL(triggered(bool)), m_grid, SLOT(setVisible(bool)));

    // cell tissue canvas
    connect(m_toolBar->m_actionShow_showCellTissue, SIGNAL(triggered(bool)), m_image, SLOT(setVisible(bool)));
    connect(m_toolBar, SIGNAL(brightnessValueChanged(qreal)), m_image, SLOT(setIntensity(qreal)));

    // legend signals
    connect(m_toolBar->m_actionShow_showLegend, SIGNAL(toggled(bool)), m_legend, SLOT(setVisible(bool)));
    connect(m_toolBar->m_actionGroup_toggleLegendPosition, SIGNAL(triggered(QAction*)), this,
            SLOT(slotSetLegendAnchor(QAction*)));

    // minimap signals
    connect(m_toolBar->m_actionShow_showMiniMap, SIGNAL(toggled(bool)), m_minimap, SLOT(setVisible(bool)));
    connect(m_toolBar->m_actionGroup_toggleMinimapPosition, SIGNAL(triggered(QAction*)), this,
            SLOT(slotSetMiniMapAnchor(QAction*)));

    // connect threshold slider to the heatmap
    connect(m_toolBar, SIGNAL(thresholdLowerValueChanged(int)), m_legend, SLOT(setLowerLimit(int)));
    connect(m_toolBar, SIGNAL(thresholdUpperValueChanged(int)), m_legend, SLOT(setUpperLimit(int)));

}

void CellViewPage::slotLoadCellFigure()
{
    DEBUG_FUNC_NAME

    DataProxy* dataProxy = DataProxy::getInstance();
    DataProxy::UserPtr current_user = dataProxy->getUser();
    DataProxy::DatasetPtr dataset = dataProxy->getDatasetById(dataProxy->getSelectedDataset());
    Q_ASSERT(!current_user.isNull() && !dataset.isNull());

    const bool forceRedFigure = (QObject::sender() == m_toolBar->m_actionShow_cellTissueRed);
    const bool forceBlueFigure = (QObject::sender() == m_toolBar->m_actionShow_cellTissueBlue);
    const bool defaultRedFigure =
        (current_user->role() == Globals::ROLE_CM)
        && !(dataset->figureStatus() & Dataset::Aligned);
    const bool loadRedFigure = (defaultRedFigure || forceRedFigure) && !forceBlueFigure;

    QString figureid = (loadRedFigure) ? dataset->figureRed() : dataset->figureBlue();
    QIODevice *device = dataProxy->getFigure(figureid);

    //read image (TODO check file is present)
    QImageReader reader(device);
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
    QPrintDialog *dialog = new QPrintDialog(&printer, this);
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
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), QDir::homePath(),
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
                       QString("%1;;%2").
                       arg(tr("Text Files (*.txt)")).
                       arg(tr("XML Files (*.xml)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }
    // append default extension
    QRegExp regex("^.*\\.(txt|xml)$", Qt::CaseInsensitive);
    if (!regex.exactMatch(filename)) {
        filename.append(".txt");
    }

    // get selected features and extend with data
    DataProxy::FeatureListPtr featureList = m_gene_plotter->getSelectedFeatures();

    // create export context
    DataProxy *dataProxy = DataProxy::getInstance();
    DataProxy::DatasetStatisticsPtr statistics = dataProxy->getStatistics(dataProxy->getSelectedDataset());

    QObject context;
    context.setProperty("hitCountMin", QVariant(statistics->min()));
    context.setProperty("hitCountMax", QVariant(statistics->max()));
    context.setProperty("hitCountSum", QVariant(statistics->hitsSum()));

    QFile textFile(filename);
    QFileInfo info(textFile);

    if (textFile.open(QFile::WriteOnly | QFile::Truncate)) {
        QObject memoryGuard;
        //TODO move intantiation to factory
        GeneExporter *exporter =
            (QString::compare(info.suffix(), "XML", Qt::CaseInsensitive) == 0) ?
            dynamic_cast<GeneExporter *>(new GeneXMLExporter(&memoryGuard)) :
            dynamic_cast<GeneExporter *>(new GeneTXTExporter(GeneTXTExporter::SimpleFull,
                                        GeneTXTExporter::TabDelimited, &memoryGuard));

        exporter->exportItem(&textFile, featureList, context);
    }
    textFile.close();
}

void CellViewPage::slotSetGeneVisualMode(QAction *action)
{
    QVariant variant = action->property("mode");
    if (variant.canConvert(QVariant::Int)) {
        Globals::GeneVisualMode mode = static_cast<Globals::GeneVisualMode>(variant.toInt());
        m_gene_plotter->setVisualMode(mode);
    } else {
        Q_ASSERT("[CellViewPage] Undefined gene visual mode!");
    }
}

void CellViewPage::slotSetMiniMapAnchor(QAction *action)
{
    QVariant variant = action->property("mode");
    if (variant.canConvert(QVariant::Int)) {
        Globals::Anchor mode = static_cast<Globals::Anchor>(variant.toInt());
        m_minimap->setAnchor(mode);
    } else {
        Q_ASSERT("[CellViewPage] Undefined anchor!");
    }
}

void CellViewPage::slotSetLegendAnchor(QAction *action)
{
    QVariant variant = action->property("mode");
    if (variant.canConvert(QVariant::Int)) {
        Globals::Anchor mode = static_cast<Globals::Anchor>(variant.toInt());
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

void CellViewPage::slotRotateView()
{
    if (QObject::sender() == m_toolBar->m_actionRotation_rotateLeft) {
      emit rotateView(-45);
    } else if (QObject::sender() == m_toolBar->m_actionRotation_rotateRight) {
      emit rotateView(45);
    } else {
      Q_ASSERT("[CellViewPage] sender is not recognized");
    }
}

void CellViewPage::slotSelectByRegExp()
{
    const DataProxy::GeneList& geneList = SelectionDialog::selectGenes(this);
    m_gene_plotter->selectGenes(geneList);
}

void CellViewPage::selectionUpdated()
{
    // gene model signals
    GeneSelectionItemModel* selectionModel =
            qobject_cast<GeneSelectionItemModel*>(ui->selections_tableview->model());
    DataProxy::FeatureListPtr features = m_gene_plotter->getSelectedFeatures();

    DataProxy *dataProxy = DataProxy::getInstance();
    DataProxy::DatasetStatisticsPtr statistics =
            dataProxy->getStatistics(dataProxy->getSelectedDataset());
    Q_ASSERT(statistics);

    selectionModel->setHitCountLimits(statistics->min(), statistics->max());
    selectionModel->loadGenes(features);
}
