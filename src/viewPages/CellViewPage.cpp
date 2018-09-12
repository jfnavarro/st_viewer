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
#include <QDateTime>
#include <QtConcurrent>

#include "viewPages/GenesWidget.h"
#include "viewPages/SpotsWidget.h"
#include "viewPages/UserSelectionsPage.h"
#include "viewRenderer/CellGLView.h"
#include "viewRenderer/CellGLView3D.h"
#include "dialogs/SelectionDialog.h"
#include "analysis/AnalysisQC.h"
#include "analysis/AnalysisClustering.h"
#include "SettingsWidget.h"
#include "SettingsStyle.h"
#include "color/HeatMap.h"
#include "viewRenderer/Window.h"
#include <algorithm>

#include "ui_cellviewPage.h"

using namespace Style;

CellViewPage::CellViewPage(QSharedPointer<SpotsWidget> spots,
                           QSharedPointer<GenesWidget> genes,
                           QSharedPointer<UserSelectionsPage> user_selections,
                           QWidget *parent)
    : QWidget(parent)
    , m_spots(spots)
    , m_genes(genes)
    , m_user_selections(user_selections)
    , m_ui(new Ui::CellView())
    , m_legend(nullptr)
    , m_gene_plotter(nullptr)
    , m_image(nullptr)
    , m_view(nullptr)
    , m_view3D(nullptr)
    , m_settings(nullptr)
    , m_dataset()

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

    // make lasso button use different icon when selected
    m_ui->lasso_selection->setStyleSheet(
                "QPushButton {border-image: url(:/images/lasso.png); } "
                "QPushButton:checked {border-image: url(:/images/lasso2.png); }");

    // instantiate Settings Widget
    m_settings.reset(new SettingsWidget());
    Q_ASSERT(!m_settings.isNull());

    // instance of clustering widget
    m_clustering.reset(new AnalysisClustering(this, Qt::Window));
    Q_ASSERT(!m_clustering.isNull());

    // initialize rendering pipeline
    initRenderer();

    // create toolbar and all the connections
    createConnections();

    // disable controls at first
    m_ui->frame->setEnabled(false);
}

CellViewPage::~CellViewPage()
{
}

void CellViewPage::clear()
{
    // reset visualization objects
    m_ui->lasso_selection->setChecked(false);
    m_ui->selection->setChecked(false);
    m_image->clearData();
    m_gene_plotter->clearData();
    m_legend->clearData();
    m_view->clearData();
    m_view->update();
    m_settings->reset();
    m_spots->clear();
    m_genes->clear();
    m_clustering->clear();
    m_clustering->close();
    m_dataset = Dataset();
}

void CellViewPage::loadDataset(const Dataset &dataset)
{    
    // reset to default
    clear();

    // update Status tip with the name of the currently selected dataset
    setStatusTip(tr("Dataset loaded %1").arg(dataset.name()));

    // update genes and spots
    m_genes->slotLoadDataset(dataset);
    m_spots->slotLoadDataset(dataset);

    // update gene plotter rendering object with the dataset
    //m_gene_plotter->clearData();
    //m_gene_plotter->attachData(dataset.data());

    // store the dataset
    m_dataset = dataset;

    // create tiles textures from the image
    m_image->clearData();
    QTransform alignment = m_dataset.imageAlignment();
    if (!dataset.imageFile().isNull() && !dataset.imageFile().isEmpty()) {
        const bool result = m_image->createTiles(dataset.imageFile());
        if (!result) {
            QMessageBox::warning(this, tr("Tissue image"), tr("Error loading tissue image"));
        } else {
            m_view->setScene(m_image->boundingRect());
            // If the user has not given any transformation matrix
            // we compute a simple transformation matrix using
            // the image and chip dimensions so the spot's coordinates
            // can be mapped to the image's coordinates space
            if (alignment.isIdentity()) {
                const QRect chip = m_dataset.chip();
                const double chip_x2 = static_cast<double>(chip.width());
                const double chip_y2 = static_cast<double>(chip.height());
                const double width_image = static_cast<double>(m_image->boundingRect().width());
                const double height_image = static_cast<double>(m_image->boundingRect().height());
                const double a11 = width_image / (chip_x2 - 1);
                const double a12 = 0.0;
                const double a13 = 0.0;
                const double a21 = 0.0;
                const double a22 = height_image / (chip_y2 - 1);
                const double a23 = 0.0;
                const double a31 = -a11;
                const double a32 = -a22;
                const double a33 = 1.0;
                alignment.setMatrix(a11, a12, a13, a21, a22, a23, a31, a32, a33);
            } else if (m_image->scaled()) {
                alignment *= QTransform::fromScale(0.5, 0.5);
            }
        }
    } else {
        m_view->setScene(m_dataset.data()->getBorder());
    }

    qDebug() << "Setting alignment matrix to " << alignment;
    m_gene_plotter->setTransform(alignment);

    // enable controls
    m_ui->frame->setEnabled(true);

    // show settings widget
    m_settings->show();

    // assigning the view object (2D or 3D)
    m_ui->mainLayout->removeWidget(m_ui->view);
    m_ui->view->close();
    if (m_dataset.data()->is3D()) {
        QWidget *widget = QWidget::createWindowContainer(m_view3D.data());
        m_ui->mainLayout->addWidget(widget);
        m_view3D->clearData();
        m_view3D->attachData(dataset.data());
        m_ui->mainLayout->update();
        widget->show();
    } else {
        m_ui->mainLayout->addWidget(m_view.data());
        m_view->clearData();
    }
}

void CellViewPage::clearSelections()
{
    m_dataset.data()->clearSelection();
    m_gene_plotter->slotUpdate();
    m_view->update();
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
        m_view3D->slotUpdate();
        m_legend->slotUpdate();
        m_view->update();
    });

    // graphic view signals
    connect(m_ui->zoomin, &QPushButton::clicked, m_view.data(), &CellGLView::zoomIn);
    connect(m_ui->zoomout, &QPushButton::clicked, m_view.data(), &CellGLView::zoomOut);

    // print canvas
    connect(m_ui->save, &QPushButton::clicked, this, &CellViewPage::slotSaveImage);
    connect(m_ui->print, &QPushButton::clicked, this, &CellViewPage::slotPrintImage);

    // selection mode
    connect(m_ui->selection, &QPushButton::clicked, [=] {
        m_view->setSelectionMode(m_ui->selection->isChecked());
    });
    connect(m_ui->lasso_selection, &QPushButton::clicked, [=] {
        m_view->setLassoSelectionMode(m_ui->lasso_selection->isChecked());
    });
    connect(m_ui->regexpselection, &QPushButton::clicked,
            this, &CellViewPage::slotSelectByRegExp);

    // view rotations
    connect(m_ui->rotate_right, &QPushButton::clicked, [=] {
        m_view->rotate(-45);
        m_view->update();
    });
    connect(m_ui->rotate_left, &QPushButton::clicked, [=] {
        m_view->rotate(45);
        m_view->update();
    });
    connect(m_ui->flip, &QPushButton::clicked, [=] {
        m_view->flip(180);
        m_view->update();
    });

    // create selection object from the selections made
    connect(m_ui->createSelection, &QPushButton::clicked,
            this, &CellViewPage::slotCreateSelection);

    // show QC widget
    connect(m_ui->histogram, &QPushButton::clicked, this, &CellViewPage::slotShowQC);

    // show Clustering widget
    connect(m_ui->clustering, &QPushButton::clicked, this, &CellViewPage::slotClustering);

    // when the user change any gene
    connect(m_genes.data(),
            &GenesWidget::signalGenesUpdated, [=] {
        m_gene_plotter->slotUpdate();
        m_view3D->slotUpdate();
        m_view->update();
    });

    // when the user change any spot
    connect(m_spots.data(),
            &SpotsWidget::signalSpotsUpdated, [=] {
        m_gene_plotter->slotUpdate();
        m_view3D->slotUpdate();
        m_view->update();
    });

    // when the user wants to load a file with spot colors
    connect(m_ui->loadSpots, &QPushButton::clicked, this, &CellViewPage::slotLoadSpotColorsFile);

    // when the user wants to load a file with genes to select
    connect(m_ui->loadGenes, &QPushButton::clicked, this, &CellViewPage::slotLoadGenes);

    // when the user clusters the spots
    connect(m_clustering.data(), &AnalysisClustering::signalClusteringUpdated,
            this, &CellViewPage::slotLoadSpotColors);

    // when the user selects spots from the clustering widget
    connect(m_clustering.data(), &AnalysisClustering::signalClusteringSpotsSelected,
            this, &CellViewPage::slotSelectSpotsClustering);

    // when the user wants to create selections from the clusters
    connect(m_clustering.data(), &AnalysisClustering::signalClusteringExportSelections,
            this, &CellViewPage::slotCreateClusteringSelections);
}


void CellViewPage::initRenderer()
{
    // the OpenGL main view objects
    m_view.reset(new CellGLView());
    m_view3D.reset(new CellGLView3D(m_settings->renderingSettings()));

    // image texture graphical object
    m_image = QSharedPointer<ImageTextureGL>(new ImageTextureGL());
    m_view->addRenderingNode(m_image);

    // gene plotter component
    m_gene_plotter = QSharedPointer<GeneRendererGL>(
                new GeneRendererGL(m_settings->renderingSettings()));
    m_view->addRenderingNode(m_gene_plotter);

    // heatmap component
    m_legend = QSharedPointer<HeatMapLegendGL>(
                new HeatMapLegendGL(m_settings->renderingSettings()));
    m_view->addRenderingNode(m_legend);
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
    QImage image = m_view->grabPixmapGL();
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

    const int quality = 100; // quality format (100 max, 0 min, -1 default)
    const QString format = fileInfo.suffix().toLower();
    QImage image = m_view->grabPixmapGL();
    if (!image.save(filename, format.toStdString().c_str(), quality)) {
        qDebug() << "Saving the image, the image coult not be saved";
    }
}

void CellViewPage::slotSelectByRegExp()
{
    SelectionDialog selectGenes(this);
    if (selectGenes.exec() == QDialog::Accepted) {
        if (selectGenes.isValid()) {
            m_dataset.data()->selectGenes(selectGenes.getRegExp(), selectGenes.selectNonVisible());
            m_gene_plotter->slotUpdate();
            m_view->update();
        }
    }
}

void CellViewPage::slotShowQC()
{
    AnalysisQC *qc = new AnalysisQC(m_dataset.data()->data(), this, Qt::Window);
    qc->show();
}

void CellViewPage::slotClustering()
{
    m_clustering->loadData(m_dataset.data()->data());
    m_clustering->show();
}

void CellViewPage::slotLoadSpotColorsFile()
{
    const QString filename
            = QFileDialog::getOpenFileName(this,
                                           tr("Open Spot Colors File"),
                                           QDir::homePath(),
                                           QString("%1").arg(tr("TXT Files (*.txt)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    QFileInfo info(filename);
    if (info.isDir() || !info.isFile() || !info.isReadable()) {
        QMessageBox::critical(this,
                              tr("Spot Colors File"),
                              tr("File is incorrect or not readable"));
        return;
    }

    QHash<QString, QColor> spotMap;
    QFile file(filename);
    bool parsed = true;
    // Parse the spots map = spot -> color
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        QString line;
        QStringList fields;
        while (!in.atEnd()) {
            line = in.readLine();
            fields = line.split("\t");
            if (fields.length() != 2) {
                parsed = false;
                break;
            }
            const QString spot = fields.at(0);
            const QColor color = Color::color_list.at(fields.at(1).toInt());
            spotMap.insert(spot, color);
        }

        if (spotMap.empty()) {
            QMessageBox::warning(this,
                                 tr("Spot Colors File"),
                                 tr("No valid spots could be found in the file"));
            parsed = false;
        }

    } else {
        QMessageBox::critical(this,
                              tr("Spot Colors File"),
                              tr("File could not be parsed"));
        parsed = false;
    }
    file.close();

    // Update spot colors
    if (parsed) {
        m_dataset.data()->loadSpotColors(spotMap);
        m_spots->update();
        m_gene_plotter->slotUpdate();
        m_view->update();
    }
}

void CellViewPage::slotLoadGenes()
{
    const QString filename
            = QFileDialog::getOpenFileName(this,
                                           tr("Open Genes File"),
                                           QDir::homePath(),
                                           QString("%1").arg(tr("TXT Files (*.txt *.tsv)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    QFileInfo info(filename);
    if (info.isDir() || !info.isFile() || !info.isReadable()) {
        QMessageBox::critical(this,
                              tr("Genes File"),
                              tr("File is incorrect or not readable"));
        return;
    }

    QHash<QString, QColor> geneMap;
    QFile file(filename);
    bool parsed = true;
    // Parse the genes map = gene -> color
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        QString line;
        QStringList fields;
        while (!in.atEnd()) {
            line = in.readLine();
            fields = line.split("\t");
            if (fields.length() != 2) {
                parsed = false;
                break;
            }
            const QString gene = fields.at(0);
            const QColor color = Color::color_list.at(fields.at(1).toInt());
            geneMap.insert(gene, color);
        }

        if (geneMap.empty()) {
            QMessageBox::warning(this,
                                 tr("Genes File"),
                                 tr("No valid genes could be found in the file"));
            parsed = false;
        }

    } else {
        QMessageBox::critical(this,
                              tr("Genes File"),
                              tr("Error parsing the file"));
        parsed = false;
    }
    file.close();

    if (parsed) {
        m_dataset.data()->loadGeneColors(geneMap);
        m_genes->update();
        m_gene_plotter->slotUpdate();
        m_view->update();
    }
}

void CellViewPage::slotLoadSpotColors()
{
    const auto spot_colors = m_clustering->getSpotClusters();
    m_dataset.data()->loadSpotColors(spot_colors);
    m_spots->update();
    m_gene_plotter->slotUpdate();
    m_view->update();
}

void CellViewPage::slotSelectSpotsClustering()
{
    m_dataset.data()->selectSpots(m_clustering->selectedSpots());
    m_gene_plotter->slotUpdate();
    m_view->update();
}

void CellViewPage::slotCreateClusteringSelections()
{
    // get the map of color -> spots
    const QMultiHash<unsigned, QString> colors_spot = m_clustering->getClustersSpot();
    // get the data frame
    auto data = m_dataset.data()->data();
    for(const auto &color : colors_spot.uniqueKeys()) {
        // get the spots for the color
        const QList<QString> &color_spots = colors_spot.values(color);
        // slice the data frame
        STData::STDataFrame scliced_data = STData::sliceDataFrameSpots(data, color_spots);
        // create selection object
        UserSelection new_selection(scliced_data);
        // proposes as selection name as DATASET NAME + color + current timestamp
        new_selection.name(m_dataset.name() + "_" + QString::number(color) + "_"
                           + QDateTime::currentDateTimeUtc().toString());
        new_selection.dataset(m_dataset.name());
        qDebug() << "Creating selection " << new_selection.name();
        m_user_selections->addSelection(new_selection);
    }
}

void CellViewPage::slotCreateSelection()
{
    // get the selected spots
    QList<QString> selected_spots =
            QtConcurrent::blockingFilteredReduced<QList<QString> >(
                m_dataset.data()->spots(),
                [] (const auto spot) { return spot->selected(); },
                [] (auto &list, const auto spot) { list.push_back(spot->name()); });
    // early out
    if (selected_spots.empty()) {
        return;
    }
    // get the data frame
    auto data = m_dataset.data()->data();
    // slice the data frame
    STData::STDataFrame scliced_data = STData::sliceDataFrameSpots(data, selected_spots);
    // create selection object
    UserSelection new_selection(scliced_data);
    // proposes as selection name as DATASET NAME plus current timestamp
    new_selection.name(m_dataset.name() + " " + QDateTime::currentDateTimeUtc().toString());
    new_selection.dataset(m_dataset.name());
    qDebug() << "Creating selection " << new_selection.name();
    m_user_selections->addSelection(new_selection);
    // clear the selection
    //clearSelections();
}
