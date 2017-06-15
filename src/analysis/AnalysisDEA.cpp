#include "AnalysisDEA.h"

#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

#include "ui_analysisDEA.h"

AnalysisDEA::AnalysisDEA(const STData::STDataFrame &data1,
                         const STData::STDataFrame &data2,
                         QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_dataA(data1)
    , m_dataB(data2)
    , m_ui(new Ui::analysisDEA)
{
    m_ui->setupUi(this);
}

AnalysisDEA::~AnalysisDEA()
{
}

void AnalysisDEA::exportTable() const
{

}

void AnalysisDEA::run()
{

}
