#include "AnalysisClustering.h"
#include "ui_analysisClustering.h"

AnalysisClustering::AnalysisClustering(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::analysisClustering)
{
    ui->setupUi(this);
}

AnalysisClustering::~AnalysisClustering()
{
    delete ui;
}
