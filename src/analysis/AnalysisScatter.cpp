#include "AnalysisScatter.h"

#include <QChartView>
#include <QValueAxis>
#include <QFileDialog>
#include <QPdfWriter>
#include <QMessageBox>
#include <QScatterSeries>

#include "math/RInterface.h"

#include "ui_analysisScatter.h"

AnalysisScatter::AnalysisScatter(const STData::STDataFrame &data,
                                 QWidget *parent) : QWidget(parent)
{

}

AnalysisScatter::~AnalysisScatter()
{

}
