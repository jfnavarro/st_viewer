#include "AnalysisCorrelation.h"

#include <QChartView>
#include <QValueAxis>
#include <QLogValueAxis>
#include <QRadioButton>
#include <QCheckBox>
#include <QSet>
#include <QMessageBox>
#include <QtMath>

#include "math/Common.h"

#include "ui_analysisCorrelation.h"

AnalysisCorrelation::AnalysisCorrelation(const STData::STDataFrame &data1,
                                         const STData::STDataFrame &data2,
                                         const QString &nameA,
                                         const QString &nameB,
                                         QWidget *parent)
    : QWidget(parent, Qt::Window)
    , m_dataA(data1)
    , m_dataB(data2)
    , m_nameA(nameA)
    , m_nameB(nameB)
    , m_ui(new Ui::analysisCorrelation)
{
    m_ui->setupUi(this);
    m_ui->exportPlot->setEnabled(false);

    // Get the shared genes (by name)
    QSet<QString> genesA = QSet<QString>(data1.genes.begin(), data1.genes.end());
    QSet<QString> genesB = QSet<QString>(data2.genes.begin(), data2.genes.end());
    const QList<QString> shared_genes = genesA.intersect(genesB).values();
    const int num_shared_genes = shared_genes.size();

    // update the shared genes field
    m_ui->sharedGenes->setText(QString::number(num_shared_genes));

    // if there are shared genes compute correlations and update
    if (num_shared_genes > 0) {
        // keep only the shared genes in the data matrix (same order)
        uvec to_keepA(num_shared_genes);
        uvec to_keepB(num_shared_genes);
        #pragma omp parallel for
        for (int i = 0; i < num_shared_genes; ++i) {
            const QString &shared_gene = shared_genes.at(i);
            to_keepA.at(i) = data1.genes.indexOf(shared_gene);
            to_keepB.at(i) = data2.genes.indexOf(shared_gene);
        }
        m_dataA.counts = m_dataA.counts.cols(to_keepA);
        m_dataA.genes = shared_genes;
        m_dataB.counts = m_dataB.counts.cols(to_keepB);
        m_dataB.genes = shared_genes;

        // create the connections
        connect(m_ui->logScale, &QCheckBox::clicked,
                this, &AnalysisCorrelation::slotUpdateData);
        connect(m_ui->exportPlot, &QPushButton::clicked,
                this, &AnalysisCorrelation::slotExportPlot);

        // Compute correlation and update the plots and data fields
        slotUpdateData();

    } else {
        QMessageBox::critical(this,
                              tr("Correlation"),
                              tr("There are no common genes between the selections"));
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

    // get the matrices of counts and log them if applies
    mat A = m_dataA.counts;
    mat B = m_dataB.counts;
    if (m_ui->logScale->isChecked()) {
        A = log1p(A);
        B = log1p(B);
    }

    // get the accumulated gene counts
    std::vector<double> rowsumA = conv_to<std::vector<double>>::from(sum(A, 0));
    std::vector<double> rowsumB = conv_to<std::vector<double>>::from(sum(B, 0));

    // compute correlation values
    const double pearson = STMath::pearson(rowsumA, rowsumB);
    m_ui->pearson->setText(QString::number(pearson));

    // create scatter plot
    m_series.reset(new QScatterSeries());
    m_series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    m_series->setMarkerSize(5.0);
    m_series->setColor(Qt::blue);
    m_series->setUseOpenGL(false);
    for (size_t i = 0; i < rowsumA.size(); ++i) {
        m_series->append(rowsumA.at(i), rowsumB.at(i));
    }
    // so users can interact with the plot
    connect(m_series.data(), &QScatterSeries::clicked,
            this, &AnalysisCorrelation::slotClickedPoint);

    m_ui->plot->setRenderHint(QPainter::Antialiasing);
    m_ui->plot->chart()->removeAllSeries();
    m_ui->plot->chart()->addSeries(m_series.data());

    // update legends in plot
    m_ui->plot->chart()->setTitle(tr("Correlation (Accumulated genes counts)"));
    m_ui->plot->chart()->setDropShadowEnabled(false);
    m_ui->plot->chart()->legend()->hide();
    m_ui->plot->chart()->createDefaultAxes();
    m_ui->plot->chart()->axes(Qt::Horizontal).first()->setTitleText("# " + m_nameA);
    m_ui->plot->chart()->axes(Qt::Vertical).first()->setTitleText("# " + m_nameB);

    m_ui->exportPlot->setEnabled(true);

    QGuiApplication::restoreOverrideCursor();
}

void AnalysisCorrelation::slotExportPlot()
{
    m_ui->plot->slotExportPlot(tr("Correlation"));
}

void AnalysisCorrelation::slotClickedPoint(const QPointF point)
{
    // Find the closest point from the series
    const double x1 = point.x();
    const double y1 = point.y();
    const auto &points = m_series->points();
    int gene_index = -1;
    //TODO make this paralell
    for (int i = 0; i < points.size(); ++i) {
        const QPointF &clicked = points.at(i);
        const double x2 = clicked.x();
        const double y2 = clicked.y();
        const double dist = STMath::euclidean(x1, y1, x2, y2);
        if (dist < 0.01) {
            gene_index = i;
            break;
        }
    }
    // Update the fied if valid gene was found
    if (gene_index != -1) {
        // genes should be the same in m_dataA and m_dataB
        m_ui->selected_gene->setText(m_dataA.genes.at(gene_index));
    }
}
