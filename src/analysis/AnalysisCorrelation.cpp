#include "AnalysisCorrelation.h"

#include <QChartView>
#include <QScatterSeries>
#include <QValueAxis>
#include <QLogValueAxis>
#include <QRadioButton>
#include <QCheckBox>
#include <QSet>

#include "math/RInterface.h"

#include "ui_analysisCorrelation.h"

AnalysisCorrelation::AnalysisCorrelation(const STData::STDataFrame &data1,
                                         const STData::STDataFrame &data2,
                                         QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_ui(new Ui::analysisCorrelation)
{
    m_ui->setupUi(this);

    // Update shared genes value
    QSet<QString> genesA = QSet<QString>::fromList(data1.genes);
    QSet<QString> genesB = QSet<QString>::fromList(data2.genes);
    const QSet<QString> shared_genes = genesA.intersect(genesB);
    m_ui->sharedGenes->setText(QString::number(shared_genes.size()));

    m_dataA = data1.counts;
    m_dataB = data2.counts;

    if (shared_genes.size() > 0) {
        // keep only the shared genes
        std::vector<uword> to_keepA;
        std::vector<uword> to_keepB;
        for (const QString &shared_gene : shared_genes) {
            to_keepA.push_back(data1.genes.indexOf(shared_gene));
            to_keepB.push_back(data2.genes.indexOf(shared_gene));
        }
        m_dataA = m_dataA.cols(uvec(to_keepA));
        m_dataB = m_dataB.cols(uvec(to_keepB));

        // Compute and cache size factors
        m_deseq_factorsA = RInterface::computeDESeqFactors(m_dataA);
        m_deseq_factorsB = RInterface::computeDESeqFactors(m_dataB);
        m_scran_factorsA = RInterface::computeScranFactors(m_dataA);
        m_scran_factorsB = RInterface::computeScranFactors(m_dataB);

        m_ui->normalization_raw->setChecked(true);

        updateData();

        connect(m_ui->normalization_raw, &QRadioButton::clicked,
                this, &AnalysisCorrelation::updateData);
        connect(m_ui->normalization_tpm, &QRadioButton::clicked,
                this, &AnalysisCorrelation::updateData);
        connect(m_ui->normalization_rel, &QRadioButton::clicked,
                this, &AnalysisCorrelation::updateData);
        connect(m_ui->normalization_deseq, &QRadioButton::clicked,
                this, &AnalysisCorrelation::updateData);
        connect(m_ui->normalization_scran, &QRadioButton::clicked,
                this, &AnalysisCorrelation::updateData);
        connect(m_ui->logScale, &QCheckBox::clicked,
                this, &AnalysisCorrelation::updateData);

    } else {
        m_ui->groupBoxNormalization->setEnabled(false);
        m_ui->logScale->setEnabled(false);
    }


}

AnalysisCorrelation::~AnalysisCorrelation()
{

}

void AnalysisCorrelation::updateData()
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

    mat A = STData::normalizeCounts(m_dataA, normalization,
                                    m_deseq_factorsA, m_scran_factorsA);
    mat B = STData::normalizeCounts(m_dataB, normalization,
                                    m_deseq_factorsB, m_scran_factorsB);

    if (m_ui->logScale->isChecked()) {
        A = log(A + 1.0);
        B = log(B + 1.0);
    }

    const std::vector<double> rowsumA = conv_to<std::vector<double>>::from(sum(A, 0));
    const std::vector<double> rowsumB = conv_to<std::vector<double>>::from(sum(B, 0));

    const double pearson = RInterface::computeCorrelation(rowsumA, rowsumB, "pearson");
    const double spearman = RInterface::computeCorrelation(rowsumA, rowsumB, "spearman");
    m_ui->pearson->setText(QString::number(pearson));
    m_ui->spearman->setText(QString::number(spearman));

    QScatterSeries *series = new QScatterSeries();
    series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    series->setMarkerSize(5.0);
    series->setColor(Qt::blue);
    series->setUseOpenGL(true);
    for (unsigned i = 0; i < rowsumA.size(); ++i) {
        series->append(rowsumA.at(i), rowsumB.at(i));
    }

    m_ui->plot->setRenderHint(QPainter::Antialiasing);
    m_ui->plot->chart()->removeAllSeries();
    m_ui->plot->chart()->addSeries(series);
    m_ui->plot->chart()->setTitle("Correlation Plot (genes)");
    m_ui->plot->chart()->setDropShadowEnabled(false);
    m_ui->plot->chart()->legend()->hide();
    m_ui->plot->chart()->createDefaultAxes();
    m_ui->plot->chart()->axisX()->setTitleText("Selection A");
    m_ui->plot->chart()->axisY()->setTitleText("Selection B");

    QGuiApplication::restoreOverrideCursor();
}
