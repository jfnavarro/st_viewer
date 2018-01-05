#include "AnalysisPCA.h"

#include <QChartView>
#include <QScatterSeries>

#include "color/HeatMap.h"
#include "math/RInterface.h"

#include "ui_AnalysisPCA.h"

AnalysisPCA::AnalysisPCA(const QList<STData::STDataFrame> &datasets,
                         const QList<QString> &names,
                         QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_ui(new Ui::AnalysisPCA)
{
    m_ui->setupUi(this);

    QSet<QString> merged_genes;
    for (unsigned i = 0; i < datasets.size(); ++i) {
        const auto data = datasets.at(i);
        merged_genes += data.genes.toSet();
    }
    QList<QString> genes = merged_genes.toList();

    const unsigned n_rows = datasets.size();
    const unsigned n_cols = merged_genes.size();
    mat merged(n_rows, n_cols);
    merged.fill(0.0);

    for (unsigned d = 0; d < datasets.size(); ++d) {
        const auto data = datasets.at(d);
        const rowvec colsums = sum(data.counts, 0);
        for (uword j = 0; j < n_cols; ++j) {
            const auto &gene = genes.at(j);
            const int index = data.genes.indexOf(gene);
            if (index != -1) {
                merged.at(d, j) = colsums.at(index);
            }
        }
    }

    mat results;
    RInterface::PCA(merged, false, true, results);

    for (unsigned d = 0; d < datasets.size(); ++d) {
        QScatterSeries *series = new QScatterSeries(this);
        series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        series->setMarkerSize(10.0);
        series->setColor(Color::color_list.at(d));
        series->setUseOpenGL(false);
        series->setName(names.at(d));
        const float x = results.at(d,0);
        const float y = results.at(d,1);
        series->append(x, y);
        m_ui->plot->chart()->addSeries(series);
    }

    const int min_x = results.col(0).min();
    const int max_x = results.col(0).max();
    const int min_y = results.col(1).min();
    const int max_y = results.col(1).max();

    m_ui->plot->setRenderHint(QPainter::Antialiasing);
    m_ui->plot->chart()->setTitle("PCA selections");
    m_ui->plot->chart()->setDropShadowEnabled(false);
    m_ui->plot->chart()->legend()->show();
    m_ui->plot->chart()->createDefaultAxes();
    m_ui->plot->chart()->axisX()->setGridLineVisible(false);
    m_ui->plot->chart()->axisX()->setLabelsVisible(true);
    m_ui->plot->chart()->axisX()->setRange(min_x - 5, max_x + 5);
    m_ui->plot->chart()->axisX()->setTitleText(tr("PCA 1"));
    m_ui->plot->chart()->axisY()->setGridLineVisible(false);
    m_ui->plot->chart()->axisY()->setLabelsVisible(true);
    m_ui->plot->chart()->axisY()->setRange(min_y - 5, max_y + 5);
    m_ui->plot->chart()->axisY()->setTitleText(tr("PCA 2"));

    connect(m_ui->exportPlot, &QPushButton::clicked, this, &AnalysisPCA::slotExportPlot);
}

AnalysisPCA::~AnalysisPCA()
{
}

void AnalysisPCA::slotExportPlot()
{
    m_ui->plot->slotExportPlot(tr("PCA plot"));
}
