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
        #pragma omp parallel for
        for (const QString &shared_gene : shared_genes) {
            to_keepA.push_back(data1.genes.indexOf(shared_gene));
            to_keepB.push_back(data2.genes.indexOf(shared_gene));
        }
        m_dataA.counts = m_dataA.counts.cols(uvec(to_keepA));
        m_dataB.counts = m_dataB.counts.cols(uvec(to_keepB));

        // create the connections
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
    m_rowsumA = conv_to<std::vector<double>>::from(sum(A, 0));
    m_rowsumB = conv_to<std::vector<double>>::from(sum(B, 0));

    // compute correlation values
    const double pearson = STMath::pearson(m_rowsumA, m_rowsumB);
    m_ui->pearson->setText(QString::number(pearson));

    // create scatter plot
    m_series.reset(new QScatterSeries());
    m_series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    m_series->setMarkerSize(5.0);
    m_series->setColor(Qt::blue);
    m_series->setUseOpenGL(false);
    #pragma omp parallel for
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
    m_ui->plot->chart()->axes(Qt::Horizontal).first()->setTitleText("# " + m_nameA);
    m_ui->plot->chart()->axes(Qt::Vertical).first()->setTitleText("# " + m_nameB);

    m_ui->exportPlot->setEnabled(true);

    QGuiApplication::restoreOverrideCursor();
}

void AnalysisCorrelation::slotExportPlot()
{
    m_ui->plot->slotExportPlot(tr("Correlation Plot"));
}

void AnalysisCorrelation::slotClickedPoint(const QPointF point)
{
    // Find the closest point from the series
    const double x1 = point.x();
    const double y1 = point.y();
    int spot_index = -1;
    #pragma omp parallel for
    for (unsigned i = 0; i < m_rowsumA.size(); ++i) {
        const double x2 = m_rowsumA.at(i);
        const double y2 = m_rowsumB.at(i);
        const double dist = STMath::euclidean(x1, y1, x2, y2);
        if (dist < 0.01) {
            spot_index = i;
            break;
        }
    }

    // Update the fied if valid gene was found
    if (spot_index != -1) {
        m_ui->selected_gene->setText(m_genes.at(spot_index));
    }
}
