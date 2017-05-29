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
#include "dialogs/SelectionDialog.h"

#include "SettingsWidget.h"
#include "SettingsStyle.h"

#include <algorithm>

#include "ui_cellviewPage.h"

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
    , m_legend(nullptr)
    , m_gene_plotter(nullptr)
    , m_image(nullptr)
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

void CellViewPage::clear()
{
    // reset visualization objects
    m_image->clearData();
    m_gene_plotter->clearData();
    m_legend->clearData();
    m_ui->view->clearData();
    m_ui->view->update();
    m_settings->reset();
}

void CellViewPage::slotLoadDataset(const Dataset &dataset)
{
    //NOTE we allow to re-open the same dataset (in case it has been edited)

    // update Status tip with the name of the currently selected dataset
    setStatusTip(tr("Dataset loaded %1").arg(dataset.name()));

    // The STData object
    auto data = dataset.data();
    data->setRenderingSettings(&m_settings->renderingSettings());

    // update gene plotter rendering object with the dataset
    m_gene_plotter->clearData();
    m_gene_plotter->attachData(data);
    //TODO set bounding rect to min and max spots

    // update SettingsWidget with the opened dataset (min max values)
    m_settings->resetReadsThreshold(data->min_reads(), data->max_reads());
    m_settings->resetTotalReadsThreshold(data->min_reads_spot(), data->max_reads_spot());
    //NOTE the total number of genes per spot can be a fixed number for the threshold
    m_settings->resetTotalGenesThreshold(1, 10000);

    // load cell tissue (to load the dataset's cell tissue image)
    // create tiles textures from the image
    m_image->clearData();
    const bool image_loaded = m_image->createTiles(dataset.imageFile());
    if (!image_loaded) {
        QMessageBox::warning(this, tr("Tissue image"),
                              tr("Error loading tissue image"));
    } else {
        m_ui->view->setScene(m_image->boundingRect());
        // Update the image aligment with the image's dimension
        // if it is not given by the user
        QTransform alignment = dataset.imageAlignment();
        if (alignment.isIdentity()) {
            // TODO 33 and 35 should be retrieved from the spots (max x and max y)
            const int width_image = m_image->boundingRect().width();
            const int height_image = m_image->boundingRect().height();
            const float a11 = width_image / 32;
            const float a12 = 0.0;
            const float a13 = 0.0;
            const float a21 = 0.0;
            const float a22 = height_image / 34;
            const float a23 = 0.0;
            const float a31 = -a11;
            const float a32 = -a22;
            const float a33 = 1.0;
            alignment.setMatrix(a11, a12, a13, a21, a22, a23, a31, a32, a33);
        }
        qDebug() << "Setting alignment matrix to " << alignment;
        m_gene_plotter->setTransform(alignment);
    }
    // call for an update
    m_ui->view->update();
}

void CellViewPage::slotClearSelections()
{
}

void CellViewPage::slotGenesUpdate()
{
    m_gene_plotter->slotUpdate();
    m_ui->view->update();
}

void CellViewPage::slotSpotsUpdated()
{
    m_gene_plotter->slotUpdate();
    m_ui->view->update();
}

void CellViewPage::createConnections()
{

    // settings menu
    connect(m_ui->genemenu, &QPushButton::clicked, m_settings.data(), &SettingsWidget::show);

    // show/hide cell image
    connect(m_settings.data(), &SettingsWidget::signalShowImage, this,
            [=](bool visible){m_image->setVisible(visible);});

    // show/hide spots
    connect(m_settings.data(), &SettingsWidget::signalShowSpots, this,
            [=](bool visible){m_gene_plotter->setVisible(visible);});

    // show/hide legend
    connect(m_settings.data(), &SettingsWidget::signalShowLegend, this,
            [=](bool visible){m_legend->setVisible(visible);});

    // rendering settings changed
    connect(m_settings.data(), &SettingsWidget::signalSpotRendering, this,
            [=](){
        m_gene_plotter->slotUpdate();
        m_legend->slotUpdate();
        m_ui->view->update();
    });

    // graphic view signals
    connect(m_ui->zoomin, &QPushButton::clicked, m_ui->view, &CellGLView::zoomIn);
    connect(m_ui->zoomout, &QPushButton::clicked, m_ui->view, &CellGLView::zoomOut);

    // print canvas
    connect(m_ui->save, &QPushButton::clicked, this, &CellViewPage::slotSaveImage);
    connect(m_ui->print, &QPushButton::clicked, this, &CellViewPage::slotPrintImage);

    // selection mode
    connect(m_ui->selection, &QPushButton::clicked, [=] {
        m_ui->view->setSelectionMode(m_ui->selection->isChecked());
    });
    connect(m_ui->regexpselection, &QPushButton::clicked,
            this, &CellViewPage::slotSelectByRegExp);

    // create selection object from the selections made
    connect(m_ui->createSelection, &QPushButton::clicked,
            this, &CellViewPage::slotCreateSelection);
}


void CellViewPage::initRenderer()
{
    // the OpenGL main view object is initialized in the UI form class

    // image texture graphical object
    m_image = QSharedPointer<ImageTextureGL>(new ImageTextureGL());
    m_ui->view->addRenderingNode(m_image);

    // gene plotter component
    m_gene_plotter = QSharedPointer<GeneRendererGL>(
                new GeneRendererGL(m_settings->renderingSettings()));
    m_ui->view->addRenderingNode(m_gene_plotter);

    // heatmap component
    m_legend = QSharedPointer<HeatMapLegendGL>(
                new HeatMapLegendGL(m_settings->renderingSettings()));
    m_ui->view->addRenderingNode(m_legend);
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
