#include "AnalysisCorrelation.h"
#include "ui_AnalysisCorrelation.h"

AnalysisCorrelation::AnalysisCorrelation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnalysisCorrelation)
{
    ui->setupUi(this);
}

AnalysisCorrelation::~AnalysisCorrelation()
{
    delete ui;
}
