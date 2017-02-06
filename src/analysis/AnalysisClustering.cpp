#include "AnalysisClustering.h"
#include "ui_analysisclustering.h"

AnalysisClustering::AnalysisClustering(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnalysisClustering)
{
    ui->setupUi(this);
}

AnalysisClustering::~AnalysisClustering()
{
    delete ui;
}
