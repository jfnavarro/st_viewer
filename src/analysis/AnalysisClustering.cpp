#include "AnalysisClustering.h"
#include "ui_analysisClustering.h"

AnalysisClustering::AnalysisClustering(const STData::STDataFrame &data,
                                       QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_data(data)
    , m_ui(new Ui::analysisClustering)
{
    m_ui->setupUi(this);

    m_ui->normalization_raw->setChecked(true);
    m_ui->dimenstions_2->setChecked(true);
    m_ui->PCA->setChecked(true);
    m_ui->kmeans->setChecked(true);

    connect(m_ui->runClustering, &QPushButton::clicked, this, &AnalysisClustering::run);
}

AnalysisClustering::~AnalysisClustering()
{
}


QList<int> AnalysisClustering::getComputedClasses() const
{
    return QList<int>();
}

void AnalysisClustering::run()
{

}
