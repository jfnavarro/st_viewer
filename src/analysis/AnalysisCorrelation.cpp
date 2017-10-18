#include "AnalysisCorrelation.h"

#include <QChartView>
#include <QValueAxis>
#include <QLogValueAxis>
#include <QRadioButton>
#include <QCheckBox>
#include <QSet>
#include <QFileDialog>
#include <QPdfWriter>
#include <QMessageBox>

#include "math/RInterface.h"

#include "ui_analysisCorrelation.h"

AnalysisCorrelation::AnalysisCorrelation(const STData::STDataFrame &data1,
                                         const STData::STDataFrame &data2,
                                         const QString &nameA,
                                         const QString &nameB,
                                         QWidget *parent,
                                         Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_ui(new Ui::analysisCorrelation)
{
    m_ui->setupUi(this);
    m_ui->exportPlot->setEnabled(false);

    // Get the shared genes (by name)
    QSet<QString> genesA = QSet<QString>::fromList(data1.genes);
    QSet<QString> genesB = QSet<QString>::fromList(data2.genes);
    const QSet<QString> shared_genes = genesA.intersect(genesB);
    const int num_shared_genes = shared_genes.size();

    // update the shared genes field
    m_ui->sharedGenes->setText(QString::number(num_shared_genes));

    // store the data
    m_dataA = data1;
    m_dataB = data2;
    m_nameA = nameA;
    m_nameB = nameB;
    m_genes = shared_genes.toList();

    if (num_shared_genes > 0) {
        // keep only the shared genes in the data matrix (same order)
        std::vector<uword> to_keepA;
        std::vector<uword> to_keepB;
        for (const QString &shared_gene : shared_genes) {
            to_keepA.push_back(data1.genes.indexOf(shared_gene));
            to_keepB.push_back(data2.genes.indexOf(shared_gene));
        }
        m_dataA.counts = m_dataA.counts.cols(uvec(to_keepA));
        m_dataB.counts = m_dataB.counts.cols(uvec(to_keepB));

        // Compute and cache size factors
        m_deseq_size_factorsA = RInterface::computeDESeqFactors(m_dataA.counts);
        m_deseq_size_factorsB = RInterface::computeDESeqFactors(m_dataB.counts);
        m_scran_size_factorsA = RInterface::computeScranFactors(m_dataA.counts, false);
        m_scran_size_factorsB = RInterface::computeScranFactors(m_dataB.counts, false);

        // default normalization is RAW
        m_ui->normalization_raw->setChecked(true);

        // create the connections
        connect(m_ui->normalization_raw, &QRadioButton::clicked,
                this, &AnalysisCorrelation::slotUpdateData);
        connect(m_ui->normalization_tpm, &QRadioButton::clicked,
                this, &AnalysisCorrelation::slotUpdateData);
        connect(m_ui->normalization_rel, &QRadioButton::clicked,
                this, &AnalysisCorrelation::slotUpdateData);
        connect(m_ui->normalization_deseq, &QRadioButton::clicked,
                this, &AnalysisCorrelation::slotUpdateData);
        connect(m_ui->normalization_scran, &QRadioButton::clicked,
                this, &AnalysisCorrelation::slotUpdateData);
        connect(m_ui->logScale, &QCheckBox::clicked,
                this, &AnalysisCorrelation::slotUpdateData);
        connect(m_ui->exportPlot, &QPushButton::clicked,
                this, &AnalysisCorrelation::slotExportPlot);

        // Update the plots and data fields
        slotUpdateData();

    } else {
        QMessageBox::critical(this,
                              tr("Correlation"),
                              tr("There are no common genes between the selections."));
        m_ui->groupBoxNormalization->setEnabled(false);
        m_ui->logScale->setEnabled(false);
        m_ui->exportPlot->setEnabled(false);
    }
}

AnalysisCorrelation::~AnalysisCorrelation()
{

}

void AnalysisCorrelation::slotUpdateData()
{
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    SettingsWidget::NormalizationMode normalization = SettingsWidget::RAW;
    if (m_ui->normalization_rel->isChecked()) {
        normalization = SettingsWidget::REL;
    } else if (m_ui->normalization_tpm->isChecked()) {
        normalization = SettingsWidget::TPM;
    } else if (m_ui->normalization_deseq->isChecked()) {
        normalization = SettingsWidget::DESEQ;
    } else if (m_ui->normalization_scran->isChecked()) {
        normalization = SettingsWidget::SCRAN;
    }

    mat A = STData::normalizeCounts(m_dataA,
                                    m_deseq_size_factorsA,
                                    m_scran_size_factorsA, normalization).counts;

    mat B = STData::normalizeCounts(m_dataB,
                                    m_deseq_size_factorsB,
                                    m_scran_size_factorsB, normalization).counts;

    if (m_ui->logScale->isChecked()) {
        A = log(A + 1.0);
        B = log(B + 1.0);
    }

    // get the accumulated gene counts
    m_rowsumA = conv_to<std::vector<double>>::from(sum(A, 0));
    m_rowsumB = conv_to<std::vector<double>>::from(sum(B, 0));

    // compute correlation values
    const double pearson = RInterface::computeCorrelation(m_rowsumA, m_rowsumB, "pearson");
    const double spearman = RInterface::computeCorrelation(m_rowsumA, m_rowsumB, "spearman");
    m_ui->pearson->setText(QString::number(pearson));
    m_ui->spearman->setText(QString::number(spearman));

    // create scatter plot
    m_series.reset(new QScatterSeries());
    m_series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    m_series->setMarkerSize(5.0);
    m_series->setColor(Qt::blue);
    m_series->setUseOpenGL(false);
    for (unsigned i = 0; i < m_rowsumA.size(); ++i) {
        m_series->append(m_rowsumA.at(i), m_rowsumB.at(i));
    }
    connect(m_series.data(), &QScatterSeries::clicked,
            this, &AnalysisCorrelation::slotClickedPoint);

    m_ui->plot->setRenderHint(QPainter::Antialiasing);
    m_ui->plot->chart()->removeAllSeries();
    m_ui->plot->chart()->addSeries(m_series.data());
    m_ui->plot->chart()->setTitle("Correlation Plot (Accumulated genes counts)");
    m_ui->plot->chart()->setDropShadowEnabled(false);
    m_ui->plot->chart()->legend()->hide();
    m_ui->plot->chart()->createDefaultAxes();
    m_ui->plot->chart()->axisX()->setTitleText("# " + m_nameA);
    m_ui->plot->chart()->axisY()->setTitleText("# " + m_nameB);

    m_ui->exportPlot->setEnabled(true);

    QGuiApplication::restoreOverrideCursor();
}

void AnalysisCorrelation::slotExportPlot()
{
    const QString filename = QFileDialog::getSaveFileName(this,
                                                          tr("Save Correlation Plot"),
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
                              tr("Save Correlation Plot"),
                              tr("The file is not writable"));
        return;
    }

    const int quality = 100; // quality format (100 max, 0 min, -1 default)
    const QString format = fileInfo.suffix().toLower();
    QImage image = m_ui->plot->grab().toImage();
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
                              tr("Save Correlation Plot"),
                              tr("The image could not be creted."));
    }
}

void AnalysisCorrelation::slotClickedPoint(const QPointF point)
{
    // Find the closest point from the series
    const QPointF clickedPoint = point;
    float closest_x = INT_MAX;
    float closest_y = INT_MAX;
    float distance = INT_MAX;
    for (const auto point : m_series->points()) {
        const float currentDistance = qSqrt((point.x() - clickedPoint.x())
                                            * (point.x() - clickedPoint.x())
                                            + (point.y() - clickedPoint.y())
                                            * (point.y() - clickedPoint.y()));
        if (currentDistance < distance) {
            distance = currentDistance;
            closest_x = point.x();
            closest_y = point.y();
        }
    }

    // Find the spot corresponding to the clicked point
    int spot_index = -1;
    for (unsigned i = 0; i < m_rowsumA.size(); ++i) {
        const float x = m_rowsumA.at(i);
        const float y = m_rowsumB.at(i);
        if (x == closest_x && y == closest_y) {
            spot_index = i;
            break;
        }
    }

    // Update the fied if valid gene was found
    if (spot_index != -1) {
        m_ui->selected_gene->setText(m_genes.at(spot_index));
    }
}
