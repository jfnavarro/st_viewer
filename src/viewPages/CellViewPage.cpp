#include "CellViewPage.h"

#include <QDebug>
#include <QString>
#include <QStringList>
#include <QPrintDialog>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QPrinter>
#include <QImageReader>
#include <QImageWriter>
#include <QPainter>

#include "viewRenderer/CellGLView.h"
#include "viewRenderer/ImageTextureGL.h"
#include "viewRenderer/HeatMapLegendGL.h"
#include "viewRenderer/GeneRendererGL.h"
#include "dialogs/SelectionDialog.h"
#include "SettingsWidget.h"
#include "GenesWidget.h"
#include "SpotsWidget.h"
#include "analysis/AnalysisFRD.h"

#include "utils/SetTips.h"
#include "SettingsVisual.h"
#include "SettingsStyle.h"

#include <algorithm>

#include "ui_cellviewPage.h"

using namespace Visual;
using namespace Style;

namespace
{

bool imageFormatHasWriteSupport(const QString &format)
{
    QStringList supportedImageFormats;
    for (auto imageformat : QImageWriter::supportedImageFormats()) {
        supportedImageFormats << QString(imageformat).toLower();
    }
    return (std::find(supportedImageFormats.begin(), supportedImageFormats.end(), format)
            != supportedImageFormats.end());
}
}

CellViewPage::CellViewPage(QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui::CellView())
    //, m_legend(nullptr)
    //, m_gene_plotter(nullptr)
    //, m_image(nullptr)
    , m_genes(nullptr)
    , m_spots(nullptr)
    , m_settings(nullptr)

{
    m_ui->setupUi(this);

    // setting style to main UI Widget (frame and widget must be set specific to avoid propagation)
    setWindowFlags(Qt::FramelessWindowHint);
    m_ui->cellViewPageWidget->setStyleSheet("QWidget#cellViewPageWidget " + PAGE_WIDGETS_STYLE);
    m_ui->frame->setStyleSheet("QFrame#frame " + PAGE_FRAME_STYLE);

    // make selection button use different icon when selected
    m_ui->selection->setStyleSheet(
        "QPushButton {border-image: url(:/images/selection.png); } "
        "QPushButton:checked {border-image: url(:/images/selection2.png); }");

    // instantiate Spots Widget
    m_spots.reset(new SpotsWidget());
    Q_ASSERT(!m_spots.isNull());

    // instantiate Genes Widget
    m_genes.reset(new GenesWidget());
    Q_ASSERT(!m_genes.isNull());

    // instantiate Settings Widget
    m_settings.reset(new SettingsWidget());
    Q_ASSERT(!m_settings.isNull());

    // initialize rendering pipeline
    initRenderer();

    // create toolbar and all the connections
    createConnections();
}

CellViewPage::~CellViewPage()
{
}

void CellViewPage::clean()
{
    // reset visualization objects
    //m_image->clearData();
    //m_gene_plotter->clearData();
    //m_legend->clearData();
    m_ui->view->clearData();
    m_ui->view->update();
    //m_genes->clearData();
    //m_spots->clearData();
    //m_settings->reset();
}

void CellViewPage::slotDatasetOpen(const Dataset &dataset)
{
    //NOTE we allow to re-open the same dataset (in case it has been edited)

    // store the dataset
    m_openedDataset = dataset;

    // update Status tip with the name of the currently selected dataset
    setStatusTip(tr("Dataset loaded %1").arg(dataset.name()));

    // update gene plotter rendering object with the dataset
    //m_gene_plotter->setDataset(dataset);

    // update Genes Widget with the opened dataset
    //m_genes->setDataset(dataset);

    // update Spots Widget with the opened dataset
    //m_spots->setDataset(dataset);

    // update Settings Widget with the opened dataset
    //m_settings->setDataset(dataset);

    // update color map legend with the dataset
    //m_legend->setDataset(dataset);

    // load cell tissue (to load the dataset's cell tissue image)
    //m_image->setDataset(dataset);
}

void CellViewPage::slotDatasetUpdated(const Dataset &dataset)
{
    Q_UNUSED(dataset)
}

void CellViewPage::slotDatasetRemoved(const Dataset &dataset)
{
    if (dataset == m_openedDataset) {
        clean();
    }
}

void CellViewPage::slotClearSelections()
{
}

void CellViewPage::createConnections()
{
/*
    // cell tissue
    connect(m_ui->actionShow_cellTissueBlue,
            SIGNAL(triggered(bool)),
            this,
            SLOT(slotLoadCellFigure()));
    connect(m_ui->actionShow_cellTissueRed,
            SIGNAL(triggered(bool)),
            this,
            SLOT(slotLoadCellFigure()));

    // log out signal
    connect(m_ui->logout, SIGNAL(clicked(bool)), this, SIGNAL(signalLogOut()));

    // graphic view signals
    connect(m_ui->zoomin, SIGNAL(clicked()), m_ui->view, SLOT(zoomIn()));
    connect(m_ui->zoomout, SIGNAL(clicked()), m_ui->view, SLOT(zoomOut()));

    // print canvas
    connect(m_ui->save, SIGNAL(clicked()), this, SLOT(slotSaveImage()));
    connect(m_ui->print, SIGNAL(clicked()), this, SLOT(slotPrintImage()));

    // selection mode
    connect(m_ui->selection, &QPushButton::clicked, [=] {
        m_ui->view->setSelectionMode(m_ui->selection->isChecked());
    });
    connect(m_ui->regexpselection, SIGNAL(clicked()), this, SLOT(slotSelectByRegExp()));

    // create selection object from the selections made
    connect(m_ui->createSelection, SIGNAL(clicked()), this, SLOT(slotCreateSelection()));

    // color selectors
    connect(m_ui->actionColor_selectColorGrid, &QAction::triggered, [=] {
        m_colorDialogGrid->show();
    });

    // gene attributes signals
    connect(m_geneIntensitySlider.data(),
            SIGNAL(valueChanged(int)),
            this,
            SLOT(slotGeneIntensity(int)));
    connect(m_geneSizeSlider.data(),
            SIGNAL(valueChanged(int)),
            this,
            SLOT(slotGeneSize(int)));
    connect(m_geneShapeComboBox.data(),
            SIGNAL(currentIndexChanged(int)),
            this,
            SLOT(slotGeneShape(int)));

    // color and pooling signals
    connect(m_colorExp.data(), &QRadioButton::clicked, [=] {
        m_legend->setColorComputingMode(ExpColor);
        m_gene_plotter->setColorComputingMode(ExpColor);
    });
    connect(m_colorLinear.data(), &QRadioButton::clicked, [=] {
        m_legend->setColorComputingMode(LinearColor);
        m_gene_plotter->setColorComputingMode(LinearColor);
    });
    connect(m_colorLog.data(), &QRadioButton::clicked, [=] {
        m_legend->setColorComputingMode(LogColor);
        m_gene_plotter->setColorComputingMode(LogColor);
    });
    connect(m_poolingGenes.data(), &QRadioButton::clicked, [=] {
        m_gene_plotter->setPoolingMode(Visual::PoolNumberGenes);
        m_legend->setPoolingMode(Visual::PoolNumberGenes);
    });
    connect(m_poolingReads.data(), &QRadioButton::clicked, [=] {
        m_gene_plotter->setPoolingMode(Visual::PoolReadsCount);
        m_legend->setPoolingMode(Visual::PoolReadsCount);
    });
    connect(m_poolingTPMs.data(), &QRadioButton::clicked, [=] {
        m_gene_plotter->setPoolingMode(Visual::PoolTPMs);
        m_legend->setPoolingMode(Visual::PoolTPMs);
    });

    // threshold slider signals
    connect(m_geneHitsThreshold.data(),
            SIGNAL(signalLowerValueChanged(int)),
            m_gene_plotter.data(),
            SLOT(setReadsLowerLimit(int)));
    connect(m_geneHitsThreshold.data(),
            SIGNAL(signalUpperValueChanged(int)),
            m_gene_plotter.data(),
            SLOT(setReadsUpperLimit(int)));
    connect(m_geneHitsThreshold.data(),
            SIGNAL(signalLowerValueChanged(int)),
            m_legend.data(),
            SLOT(setReadsLowerLimit(int)));
    connect(m_geneHitsThreshold.data(),
            SIGNAL(signalUpperValueChanged(int)),
            m_legend.data(),
            SLOT(setReadsUpperLimit(int)));

    connect(m_geneGenesThreshold.data(),
            SIGNAL(signalLowerValueChanged(int)),
            m_gene_plotter.data(),
            SLOT(setGenesLowerLimit(int)));
    connect(m_geneGenesThreshold.data(),
            SIGNAL(signalUpperValueChanged(int)),
            m_gene_plotter.data(),
            SLOT(setGenesUpperLimit(int)));
    connect(m_geneGenesThreshold.data(),
            SIGNAL(signalLowerValueChanged(int)),
            m_legend.data(),
            SLOT(setGenesLowerLimit(int)));
    connect(m_geneGenesThreshold.data(),
            SIGNAL(signalUpperValueChanged(int)),
            m_legend.data(),
            SLOT(setGenesUpperLimit(int)));

    connect(m_geneTotalReadsThreshold.data(),
            SIGNAL(signalLowerValueChanged(int)),
            m_gene_plotter.data(),
            SLOT(setTotalReadsLowerLimit(int)));
    connect(m_geneTotalReadsThreshold.data(),
            SIGNAL(signalUpperValueChanged(int)),
            m_gene_plotter.data(),
            SLOT(setTotalReadsUpperLimit(int)));

    // show/not genes signal
    connect(m_ui->actionShow_showGenes,
            SIGNAL(triggered(bool)),
            m_gene_plotter.data(),
            SLOT(setVisible(bool)));

    // enable/disable genes individual cut-off
    connect(m_ui->actionIndividual_gene_cut_off,
            SIGNAL(triggered(bool)),
            m_gene_plotter.data(),
            SLOT(slotSetGenesCutOff(bool)));

    // visual mode signal
    connect(actionGroup_toggleVisualMode,
            SIGNAL(triggered(QAction *)),
            this,
            SLOT(slotSetGeneVisualMode(QAction *)));

    // grid signals
    connect(m_colorDialogGrid.data(),
            SIGNAL(colorSelected(const QColor &)),
            m_grid.data(),
            SLOT(setColor(const QColor &)));
    connect(m_ui->actionShow_showGrid,
            SIGNAL(triggered(bool)),
            m_grid.data(),
            SLOT(setVisible(bool)));

    // cell tissue canvas
    connect(m_ui->actionShow_showCellTissue,
            SIGNAL(triggered(bool)),
            m_image.data(),
            SLOT(setVisible(bool)));

    // legend signals
    connect(m_ui->actionShow_showLegend,
            SIGNAL(toggled(bool)),
            m_legend.data(),
            SLOT(setVisible(bool)));
    connect(actionGroup_toggleLegendPosition,
            SIGNAL(triggered(QAction *)),
            this,
            SLOT(slotSetLegendAnchor(QAction *)));

    // Features Histogram Distribution
    connect(m_geneHitsThreshold.data(),
            SIGNAL(signalLowerValueChanged(int)),
            m_FDH.data(),
            SLOT(setLowerLimit(int)));
    connect(m_geneHitsThreshold.data(),
            SIGNAL(signalUpperValueChanged(int)),
            m_FDH.data(),
            SLOT(setUpperLimit(int)));
    connect(m_ui->histogram, &QPushButton::clicked, [=] { m_FDH->show(); });
  */
}


void CellViewPage::initRenderer()
{/*    // the OpenGL main view object is initialized in the UI form class

    // image texture graphical object
    m_image = QSharedPointer<ImageTextureGL>(new ImageTextureGL());
    m_ui->view->addRenderingNode(m_image);

    // gene plotter component
    m_gene_plotter = QSharedPointer<GeneRendererGL>(new GeneRendererGL(m_dataProxy));
    m_ui->view->addRenderingNode(m_gene_plotter);

    // heatmap component
    m_legend = QSharedPointer<HeatMapLegendGL>(new HeatMapLegendGL());
    m_legend->setAnchor(DEFAULT_ANCHOR_LEGEND);
    m_ui->view->addRenderingNode(m_legend);
*/
}

void CellViewPage::slotPrintImage()
{
    QPrinter printer;
    printer.setColorMode(QPrinter::Color);
    printer.setOrientation(QPrinter::Landscape);

    QScopedPointer<QPrintDialog> dialog(new QPrintDialog(&printer, this));
    if (dialog->exec() != QDialog::Accepted) {
        return;
    }

    QPainter painter(&printer);
    QRect rect = painter.viewport();
    QImage image = m_ui->view->grabPixmapGL();
    QSize size = image.size();
    size.scale(rect.size(), Qt::KeepAspectRatio);
    painter.setViewport(QRect(QPoint(0, 0), size));
    painter.setWindow(image.rect());
    painter.drawImage(0, 0, image);
}

void CellViewPage::slotSaveImage()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save Image"),
                                                    QDir::homePath(),
                                                    QString("%1;;%2;;%3")
                                                        .arg(tr("JPEG Image Files (*.jpg *.jpeg)"))
                                                        .arg(tr("PNG Image Files (*.png)"))
                                                        .arg(tr("BMP Image Files (*.bmp)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    const QFileInfo fileInfo(filename);
    const QFileInfo dirInfo(fileInfo.dir().canonicalPath());
    if (!fileInfo.exists() && !dirInfo.isWritable()) {
        qDebug() << "Saving the image, the directory is not writtable";
        return;
    }

    const QString format = fileInfo.suffix().toLower();
    if (!imageFormatHasWriteSupport(format)) {
        // This should never happen because getSaveFileName() automatically
        // adds the suffix from the "Save as type" choosen.
        // But this would be triggered if somehow there is no jpg, png or bmp support
        // compiled in the application
        qDebug() << "Saving the image, the image format is not supported";
        return;
    }

    const int quality = 100; // quality format (100 max, 0 min, -1 default)
    QImage image = m_ui->view->grabPixmapGL();
    if (!image.save(filename, format.toStdString().c_str(), quality)) {
        qDebug() << "Saving the image, the image coult not be saved";
    }
}

void CellViewPage::slotSelectByRegExp()
{
    //const STData::gene_list &geneList = SelectionDialog::selectGenes(m_openedDataset->genes());
    //m_openedDataset->seletGenes(geneList);
}

void CellViewPage::slotCreateSelection()
{/*
    // get selected features and create the selection object
    const auto &selectedSpots = m_gene_plotter->getSelectedSpots();
    if (selectedSpots.empty()) {
        // the user has probably clear the selections
        return;
    }
    // create a copy of the Dataet slicing by the selected spots
    Dataset dataset_copy = m_openedDataset.sliceSpots(selectedSpots);
    // create selection object
    UserSelection new_selection;
    new_selection.dataset(dataset_copy);
    new_selection.type(UserSelection::Rubberband);
    // proposes as selection name as DATASET NAME plus current timestamp
    new_selection.name(m_openedDataset.name() + " " + QDateTime::currentDateTimeUtc().toString());
    // add image snapshot of the main canvas
    QImage tissue_snapshot = m_ui->view->grabPixmapGL();
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    tissue_snapshot.save(&buffer, "JPG");
    new_selection.tissueSnapShot(ba.toBase64());
    // clear the selection in gene plotter
    m_gene_plotter->clearSelection();
    // notify that the selection was created and added locally
    emit signalUserSelection(UserSelection);*/
}
