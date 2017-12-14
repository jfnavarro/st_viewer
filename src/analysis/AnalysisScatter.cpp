#include "AnalysisScatter.h"

#include <QChartView>
#include <QValueAxis>
#include <QFileDialog>
#include <QPdfWriter>
#include <QMessageBox>
#include <QScatterSeries>

#include "color/HeatMap.h"

#include "ui_analysisScatter.h"

AnalysisScatter::AnalysisScatter(const STData::STDataFrame &data,
                                 QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_ui(new Ui::analysisScatter)
{

    // setup UI
    m_ui->setupUi(this);

    const unsigned num_spots = data.spots.size();
    const colvec spot_reads = sum(data.counts, 1);
    const colvec spot_genes = STData::computeNonZeroRows(data.counts);
    const float min_reads = spot_reads.min();
    const float max_reads = spot_reads.max();
    const float min_genes = spot_genes.min();
    const float max_genes = spot_genes.max();
    for (unsigned i = 0; i < num_spots; ++i) {
        QScatterSeries *series_reads = new QScatterSeries(this);
        QScatterSeries *series_genes = new QScatterSeries(this);
        series_reads->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        series_reads->setMarkerSize(10.0);
        series_reads->setUseOpenGL(false);

        series_genes->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        series_genes->setMarkerSize(10.0);
        series_genes->setUseOpenGL(false);

        const auto &spot = Spot::getCoordinates(data.spots.at(i));
        const float value_reads = spot_reads.at(i);
        const float value_genes = spot_genes.at(i);
        const QColor color_reads = Color::createCMapColor(value_reads, min_reads,
                                                          max_reads, Color::ColorGradients::gpHot);
        const QColor color_genes = Color::createCMapColor(value_genes, min_genes,
                                                          max_genes, Color::ColorGradients::gpHot);
        series_reads->setColor(color_reads);
        series_reads->append(spot.first, spot.second * -1);
        m_ui->plotReads->chart()->addSeries(series_reads);

        series_genes->setColor(color_genes);
        series_genes->append(spot.first, spot.second * -1);
        m_ui->plotGenes->chart()->addSeries(series_genes);
    }

    m_ui->plotReads->chart()->setTitle("Spots colored by expression (transcripts)");
    m_ui->plotReads->chart()->setDropShadowEnabled(false);
    m_ui->plotReads->chart()->legend()->hide();
    m_ui->plotReads->chart()->createDefaultAxes();

    m_ui->plotGenes->chart()->setTitle("Spots colored by expression (genes)");
    m_ui->plotGenes->chart()->setDropShadowEnabled(false);
    m_ui->plotGenes->chart()->legend()->hide();
    m_ui->plotGenes->chart()->createDefaultAxes();

    connect(m_ui->exportReads, &QPushButton::clicked, this, &AnalysisScatter::slotExportPlotReads);
    connect(m_ui->exportGenes, &QPushButton::clicked, this, &AnalysisScatter::slotExportPlotGenes);
}

AnalysisScatter::~AnalysisScatter()
{

}

void AnalysisScatter::slotExportPlotReads()
{
    const QString filename = QFileDialog::getSaveFileName(this,
                                                          tr("Save Scatter Transcripts Plot"),
                                                          QDir::homePath(),
                                                          QString("%1;;%2;;%3;;%4")
                                                          .arg(tr("JPEG Image Files (*.jpg *.jpeg)"))
                                                          .arg(tr("PNG Image Files (*.png)"))
                                                          .arg(tr("BMP Image Files (*.bmp)"))
                                                          .arg(tr("PDF Image Files (*.pdf)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    const QFileInfo fileInfo(filename);
    const QFileInfo dirInfo(fileInfo.dir().canonicalPath());
    if (!fileInfo.exists() && !dirInfo.isWritable()) {
        QMessageBox::critical(this,
                              tr("Save Scatter Transcripts Plot"),
                              tr("The file is not writable"));
        return;
    }

    const int quality = 100; // quality format (100 max, 0 min, -1 default)
    const QString format = fileInfo.suffix().toLower();
    QImage image = m_ui->plotReads->grab().toImage();
    if (format.toLower().contains("pdf")) {
        QPdfWriter writer(filename);
        const QPageSize size(image.size(), QPageSize::Unit::Millimeter, "custom");
        writer.setPageSize(size);
        writer.setResolution(25);
        writer.setPageMargins(QMarginsF(0,0,0,0));
        QPainter painter(&writer);
        painter.drawImage(0,0, image);
    } else if (!image.save(filename, format.toStdString().c_str(), quality)) {
        QMessageBox::critical(this,
                              tr("Save Scatter Transcripts Plot"),
                              tr("The image could not be creted."));
    }
}

void AnalysisScatter::slotExportPlotGenes()
{
    const QString filename = QFileDialog::getSaveFileName(this,
                                                          tr("Save Scatter Genes Plot"),
                                                          QDir::homePath(),
                                                          QString("%1;;%2;;%3;;%4")
                                                          .arg(tr("JPEG Image Files (*.jpg *.jpeg)"))
                                                          .arg(tr("PNG Image Files (*.png)"))
                                                          .arg(tr("BMP Image Files (*.bmp)"))
                                                          .arg(tr("PDF Image Files (*.pdf)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    const QFileInfo fileInfo(filename);
    const QFileInfo dirInfo(fileInfo.dir().canonicalPath());
    if (!fileInfo.exists() && !dirInfo.isWritable()) {
        QMessageBox::critical(this,
                              tr("Save Scatter Genes Plot"),
                              tr("The file is not writable"));
        return;
    }

    const int quality = 100; // quality format (100 max, 0 min, -1 default)
    const QString format = fileInfo.suffix().toLower();
    QImage image = m_ui->plotGenes->grab().toImage();
    if (format.toLower().contains("pdf")) {
        QPdfWriter writer(filename);
        const QPageSize size(image.size(), QPageSize::Unit::Millimeter, "custom");
        writer.setPageSize(size);
        writer.setResolution(25);
        writer.setPageMargins(QMarginsF(0,0,0,0));
        QPainter painter(&writer);
        painter.drawImage(0,0, image);
    } else if (!image.save(filename, format.toStdString().c_str(), quality)) {
        QMessageBox::critical(this,
                              tr("Save Scatter Genes Plot"),
                              tr("The image could not be creted."));
    }
}
