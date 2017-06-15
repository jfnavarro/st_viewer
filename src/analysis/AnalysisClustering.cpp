#include "AnalysisClustering.h"

#include <QChartView>
#include <QScatterSeries>
#include <QValueAxis>

#include "math/RInterface.h"

#include "ui_analysisClustering.h"

AnalysisClustering::AnalysisClustering(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_ui(new Ui::analysisClustering)
{
    // setup UI
    m_ui->setupUi(this);
    m_ui->normalization_raw->setChecked(true);
    m_ui->theta->setValue(0.5);
    connect(m_ui->runClustering, &QPushButton::clicked, this, &AnalysisClustering::run);
}

AnalysisClustering::~AnalysisClustering()
{
}


QVector<QColor> AnalysisClustering::getComputedClasses() const
{
    return m_computed_colors;
}

void AnalysisClustering::loadData(const STData::STDataFrame &data)
{
    m_data = data;

    // initialize the color vector to the number of spots
    m_computed_colors = QVector<QColor>(m_data.counts.n_rows);

    // compute size factors for normalization
    m_deseq_factors = RInterface::computeDESeqFactors(m_data.counts);
    m_scran_factors = RInterface::computeScranFactors(m_data.counts);
}

void AnalysisClustering::run()
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

    // Normalize and log matrix of counts
    mat A = STData::normalizeCounts(m_data.counts, normalization,
                                    m_deseq_factors, m_scran_factors);
    if (m_ui->logScale->isChecked()) {
        A = log(A + 1.0);
    }

    const int no_dims = 2;
    const int perplexity = m_ui->perplexity->value();
    const double theta = m_ui->theta->value();
    const int max_iter = m_ui->max_iter->value();
    const int init_dim = m_ui->init_dims->value();
    const int num_clusters = m_ui->clusters->value();

    std::vector<int> colors;
    mat reduced_coordinates;
    //Surprisingly it is much faster to call R's tsne than to use C++ implemtation....
    RInterface::spotClassification(A, num_clusters, init_dim, no_dims, perplexity,
                                   max_iter, theta, colors, reduced_coordinates);

    // Create one serie for each different cluster (color)
    QStringList color_list;
    color_list << "red" << "green" << "blue" << "cyan" << "magenta"
               << "yellow" << "black" << "grey" << "darkBlue" << "darkGreen";
    QVector<QScatterSeries *> series_vector;
    for (int k = 0; k < num_clusters; ++k) {
        QScatterSeries *series = new QScatterSeries();
        series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        series->setMarkerSize(10.0);
        series->setColor(color_list.at(k));
        series->setUseOpenGL(true);
        series_vector.push_back(series);
    }

    const std::vector<double> x = conv_to<std::vector<double>>::from(reduced_coordinates.col(0));
    const std::vector<double> y = conv_to<std::vector<double>>::from(reduced_coordinates.col(1));
    // add the respective spot (t-SNE coordinates) to the serie it belongs to
    for (unsigned i = 0; i < colors.size(); ++i) {
        const int k = colors.at(i);
        m_computed_colors[i] = color_list.at(k);
        series_vector[k - 1]->append(x.at(i), y.at(i));
    }

    // update the scatter plot
    m_ui->plot->setRenderHint(QPainter::Antialiasing);
    m_ui->plot->chart()->removeAllSeries();
    for (auto series : series_vector) {
        m_ui->plot->chart()->addSeries(series);
    }
    m_ui->plot->chart()->setTitle("Spots colored by cluster");
    m_ui->plot->chart()->setDropShadowEnabled(false);
    m_ui->plot->chart()->legend()->show();
    m_ui->plot->chart()->createDefaultAxes();

    // notify the main view
    emit singalClusteringUpdated();

    QGuiApplication::restoreOverrideCursor();
}
