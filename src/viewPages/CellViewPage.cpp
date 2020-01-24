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
#include "viewRenderer/CellGLView3D.h"
#include "dialogs/SelectionDialog.h"
#include "analysis/AnalysisQC.h"
#include "analysis/AnalysisClustering.h"
#include "SettingsWidget.h"
#include "SettingsStyle.h"
#include "color/HeatMap.h"

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

    // attach visual settings
    m_ui->view->attachSettings(&m_settings->renderingSettings());

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
    m_ui->view->clearData();
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

    // store the dataset
    m_dataset = dataset;

    // enable controls
    m_ui->frame->setEnabled(true);
    m_ui->histogram->setEnabled(!dataset.data()->is3D());
    m_ui->clustering->setEnabled(!dataset.data()->is3D());
    m_ui->selection->setEnabled(!dataset.data()->is3D());
    m_ui->lasso_selection->setEnabled(!dataset.data()->is3D());
    m_ui->rotate_left->setEnabled(!dataset.data()->is3D());
    m_ui->rotate_right->setEnabled(!dataset.data()->is3D());
    m_ui->flip->setEnabled(!dataset.data()->is3D());
    m_ui->createSelection->setEnabled(!dataset.data()->is3D());
    m_ui->zoomin->setEnabled(!dataset.data()->is3D());
    m_ui->zoomout->setEnabled(!dataset.data()->is3D());
    m_settings->slotShowImageEnabled(!dataset.data()->is3D());

    // show settings widget
    m_settings->show();

    // clear view and attach data object
    m_ui->view->clearData();
    m_ui->view->attachDataset(dataset);
    m_ui->view->show();
}

void CellViewPage::clearSelections()
{
    m_dataset.data()->clearSelection();
    m_ui->view->slotUpdate();
}

void CellViewPage::createConnections()
{

    // settings menu
    connect(m_ui->genemenu, &QPushButton::clicked, m_settings.data(), &SettingsWidget::show);

    // show/hide cell image
    connect(m_settings.data(), &SettingsWidget::signalShowImage, this,
            [=](bool visible){m_ui->view->slotImageVisible(visible);});

    // Invoke a rendering
    connect(m_settings.data(), &SettingsWidget::signalRendering, this,
            [=](){ m_ui->view->update(); });

    // show/hide legend
    connect(m_settings.data(), &SettingsWidget::signalShowLegend, this,
            [=](bool visible){m_ui->view->slotLegendVisible(visible);});

    // rendering settings changed
    connect(m_settings.data(), &SettingsWidget::signalSpotRendering, this,
            [=](){ m_ui->view->slotUpdate(); });

    // graphic view signals
    connect(m_ui->zoomin, &QPushButton::clicked, m_ui->view, &CellGLView3D::slotZoomIn);
    connect(m_ui->zoomout, &QPushButton::clicked, m_ui->view, &CellGLView3D::slotZoomOut);

    // print canvas
    connect(m_ui->save, &QPushButton::clicked, this, &CellViewPage::slotSaveImage);
    connect(m_ui->print, &QPushButton::clicked, this, &CellViewPage::slotPrintImage);

    // selection mode
    connect(m_ui->selection, &QPushButton::clicked, [=] {
        m_ui->view->slotRubberBandSelectionMode(m_ui->selection->isChecked());
    });
    connect(m_ui->lasso_selection, &QPushButton::clicked, [=] {
        m_ui->view->slotLassoSelectionMode(m_ui->lasso_selection->isChecked());
    });

    // view rotations
    connect(m_ui->rotate_right, &QPushButton::clicked, [=] {
        m_ui->view->slotRotate(-45);
    });
    connect(m_ui->rotate_left, &QPushButton::clicked, [=] {
        m_ui->view->slotRotate(45);
    });
    connect(m_ui->flip, &QPushButton::clicked, [=] {
        m_ui->view->slotFlip(180);
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
        m_ui->view->slotUpdate();
    });

    // when the user change any spot
    connect(m_spots.data(),
            &SpotsWidget::signalSpotsUpdated, [=] {
        m_ui->view->slotUpdate();
    });

    // when the user wants to load a file with spot colors
    connect(m_ui->loadSpots, &QPushButton::clicked, this, &CellViewPage::slotLoadSpotClustersFile);

    // when the user wants to load a file with genes to select
    connect(m_ui->loadGenes, &QPushButton::clicked, this, &CellViewPage::slotLoadGenesColors);

    // when the user clusters the spots
    connect(m_clustering.data(), &AnalysisClustering::signalClusteringUpdated,
            this, &CellViewPage::slotLoadSpotClusters);

    // when the user selects spots from the clustering widget
    connect(m_clustering.data(), &AnalysisClustering::signalClusteringSpotsSelected,
            this, &CellViewPage::slotSelectSpotsClustering);

    // when the user wants to create selections from the clusters
    connect(m_clustering.data(), &AnalysisClustering::signalClusteringExportSelections,
            this, &CellViewPage::slotCreateClusteringSelections);
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

    const int quality = 100; // quality format (100 max, 0 min, -1 default)
    const QString format = fileInfo.suffix().toLower();
    QImage image = m_ui->view->grabPixmapGL();
    if (!image.save(filename, format.toStdString().c_str(), quality)) {
        qDebug() << "Saving the image, the image coult not be saved";
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

//TODO same code as slotLoadGenesColors()
void CellViewPage::slotLoadSpotClustersFile()
{
    const QString filename
            = QFileDialog::getOpenFileName(this,
                                           tr("Open Spot Clusters File"),
                                           QDir::homePath(),
                                           QString("%1").arg(tr("TXT Files (*.txt *.tsv)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    QFileInfo info(filename);
    if (info.isDir() || !info.isFile() || !info.isReadable()) {
        QMessageBox::critical(this,
                              tr("Spot Clusters File"),
                              tr("File is incorrect or not readable"));
        return;
    }

    QFile file(filename);
    QList<QString> spots;
    QList<int> clusters;
    QList<QString> infos;
    bool parsed = true;
    // Parse the spots map = spot -> color
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        QString line;
        QStringList fields;
        while (!in.atEnd()) {
            line = in.readLine();
            fields = line.split("\t");
            if (fields.length() < 2) {
                parsed = false;
                break;
            }
            const QString spot = fields.at(0);
            const int cluster = fields.at(1).toInt();
            const QString info = fields.size() > 2 ? fields.at(2) : QString::number(cluster);
            spots.append(spot);
            clusters.append(cluster);
            infos.append(info);
        }

        if (spots.empty()) {
            QMessageBox::warning(this,
                                 tr("Spot Clusters File"),
                                 tr("No valid spots could be found in the file"));
            parsed = false;
        }

    } else {
        QMessageBox::critical(this,
                              tr("Spot Clusters File"),
                              tr("File could not be parsed"));
        parsed = false;
    }
    file.close();

    // Update spot colors
    if (parsed) {
        m_dataset.data()->loadSpotColors(spots, clusters, infos);
        m_spots->update();
        m_ui->view->slotUpdate();
    }
}

void CellViewPage::slotLoadGenesColors()
{
    const QString filename
            = QFileDialog::getOpenFileName(this,
                                           tr("Open Genes Colors"),
                                           QDir::homePath(),
                                           QString("%1").arg(tr("TXT Files (*.txt *.tsv)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    QFileInfo info(filename);
    if (info.isDir() || !info.isFile() || !info.isReadable()) {
        QMessageBox::critical(this,
                              tr("Genes File Colors"),
                              tr("File is incorrect or not readable"));
        return;
    }

    QFile file(filename);
    QList<QString> genes;
    QList<int> colors;
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
            const int color = fields.at(1).toInt();
            genes.append(gene);
            colors.append(color);
        }

        if (genes.empty()) {
            QMessageBox::warning(this,
                                 tr("Genes File Colors"),
                                 tr("No valid genes could be found in the file"));
            parsed = false;
        }

    } else {
        QMessageBox::critical(this,
                              tr("Genes File Colors"),
                              tr("Error parsing the file"));
        parsed = false;
    }
    file.close();

    // Update gene colors
    if (parsed) {
        m_dataset.data()->loadGeneColors(genes, colors);
        m_genes->update();
        m_ui->view->slotUpdate();
    }
}

void CellViewPage::slotLoadSpotClusters()
{
    QList<QString> spots;
    QList<int> clusters;
    QList<QString> infos;
    for (const auto &cluster : m_clustering->getSpotClusters()) {
        spots.append(cluster.first);
        clusters.append(cluster.second);
        infos.append(QString::number(cluster.second));
    }
    m_dataset.data()->loadSpotColors(spots,
                                     clusters,
                                     infos);
    m_spots->update();
    m_ui->view->slotUpdate();
}

void CellViewPage::slotSelectSpotsClustering()
{
    m_dataset.data()->selectSpots(m_clustering->selectedSpots());
    m_ui->view->slotUpdate();
}

void CellViewPage::slotCreateClusteringSelections()
{
    // get the map of color -> spots
    const QMultiHash<int, QString> colors_spot = m_clustering->getClustersSpot();
    for(const auto &color : colors_spot.uniqueKeys()) {
        // get the spots for the color
        const QList<QString> &color_spots = colors_spot.values(color);
        // slice the data frame
        STData::STDataFrame scliced_data = m_dataset.data()->sliceDataSpots(color_spots);
        //TODO check the the slice is not empty
        // create selection object
        UserSelection new_selection(scliced_data);
        // proposes a selection name as DATASET NAME + color + current timestamp
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
    // slice the data frame
    STData::STDataFrame scliced_data = m_dataset.data()->sliceDataSpots(selected_spots);
    //TODO check the the slice is not empty
    // create selection object
    UserSelection new_selection(scliced_data);
    // proposes as selection name as DATASET NAME plus current timestamp
    new_selection.name(m_dataset.name() + " " + QDateTime::currentDateTimeUtc().toString());
    new_selection.dataset(m_dataset.name());
    qDebug() << "Creating selection " << new_selection.name();
    m_user_selections->addSelection(new_selection);
    m_dataset.data()->clearSelection();
    m_ui->view->slotUpdate();
}
