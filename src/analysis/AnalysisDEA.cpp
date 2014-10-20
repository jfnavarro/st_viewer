/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "AnalysisDEA.h"

#include "ui_ddaWidget.h"

#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

#include <cmath>
#include "math/Common.h"

AnalysisDEA::AnalysisDEA(QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    m_ui(new Ui::ddaWidget),
    m_customPlot(nullptr)
{
    setModal(true);

    m_ui->setupUi(this);

    // creating plotting object
    m_customPlot = new QCustomPlot(m_ui->plotWidget);
    Q_ASSERT(m_customPlot != nullptr);

    // add a scatter plot graph
    m_customPlot->addGraph();
    m_customPlot->graph(0)->setScatterStyle(
                QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::blue), Qt::white, 5));
    m_customPlot->graph(0)->setAntialiasedScatters(true);
    m_customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
    m_customPlot->graph(0)->setName("Correlation Scatter Plot");
    m_customPlot->graph(0)->rescaleAxes(true);

    // sets the legend and attributes
    m_customPlot->legend->setVisible(false);
    m_customPlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
    m_customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
    m_customPlot->xAxis->setTicks(true);
    m_customPlot->yAxis->setTicks(true);
    // make top right axes clones of bottom left axes. Looks prettier:
    m_customPlot->axisRect()->setupFullAxesBox();
    // plot and add mouse interaction (fixed size)
    m_customPlot->setFixedSize(500, 400);
    m_customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    //make connections
    connect(m_ui->cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(m_ui->saveButton, SIGNAL(clicked()), this, SLOT(saveToPDF()));
}

AnalysisDEA::~AnalysisDEA()
{
    m_customPlot->deleteLater();
    m_customPlot = nullptr;
}

//TODO split and optimize this function
void AnalysisDEA::computeData(const GeneSelection &selObjectA,
                          const GeneSelection &selObjectB)
{
    typedef QMap<QString, QPair<qreal,qreal> > geneToNormalizedPairType;

    // get the list of selected items
    const auto &selA = selObjectA.selectedItems();
    const auto &selB = selObjectB.selectedItems();

    // get the size of the biggest list
    const int selection1Size = selA.size();
    const int selection2Size = selB.size();
    const int biggestSize = qMax(selection1Size, selection2Size);

    //take into account that some genes might be present in only one selection
    //therefore, we create a hash table (key gene name - value a pairt with value in selection A
    //and value in selection B) to later know what genes are present in which set
    //depending of the value of the hash (0.0 no present)
    geneToNormalizedPairType genesToNormalizedReads;
    for (int i = 0; i < biggestSize; ++i) {
        if (selection1Size > i) {
            const auto& selection1 = selA.at(i);
            genesToNormalizedReads[selection1.name].first = selection1.reads;
        }
        if (selection2Size > i) {
            const auto& selection2 = selB.at(i);
            genesToNormalizedReads[selection2.name].second = selection2.reads;
        }
    }

    // clear the table
    m_ui->tableWidget->clear();
    // initialize columns and headers of the table
    m_ui->tableWidget->setColumnCount(3);
    QStringList headers;
    headers << "Gene" << "Reads Sel. A" << "Reads Sel. B";
    m_ui->tableWidget->setHorizontalHeaderLabels(headers);
    // initialize row size of the table
    m_ui->tableWidget->setRowCount(biggestSize);
    int index = 0; //to keep count of the elements inserted

    // create vector containers for plot data points and temp variables for computing stats
    QVector<double> valuesSelectionA;
    QVector<double> valuesSelectionB;
    QVector<double> loggedValuesSelectionA;
    QVector<double> loggedValuesSelectionB;
    QVector<double> nonNormalizedvaluesA;
    QVector<double> nonNormalizedvaluesB;
    unsigned countAB = 0;
    unsigned countA = 0;
    unsigned countB = 0;

    //total reads in each selection used to normalize
    const int totalReadsSelectionA = selObjectA.totalReads();
    const int totalReadsSelectionB = selObjectB.totalReads();

    //iterate the hash table to compute the DDA stats and populate the table
    geneToNormalizedPairType::const_iterator end = genesToNormalizedReads.end();
    for (geneToNormalizedPairType::const_iterator it = genesToNormalizedReads.begin();
         it != end; ++it) {

        const qreal valueSelection1 = it.value().first;
        const qreal valueSelection2 = it.value().second;

        // update table
        m_ui->tableWidget->setItem(index, 0, new TableItem(it.key()));
        m_ui->tableWidget->setItem(index, 1, new TableItem(valueSelection1));
        m_ui->tableWidget->setItem(index, 2, new TableItem(valueSelection2));
        index++;

        // compute overlapping counting values
        if (valueSelection1 == 0.0) {
            countB++;
        } else if (valueSelection2 == 0.0) {
            countA++;
        } else {
            countAB++;
        }

        // populate lists of values with normalized values (for the scatter plot)
        const qreal normalizedValueSelection1 =
                ((valueSelection1 * 10e5) / totalReadsSelectionA) + 1;
        const qreal normalizedValueSelection2 =
                ((valueSelection2 * 10e5) / totalReadsSelectionB) + 1;

        // update lists of values
        valuesSelectionA.push_back(normalizedValueSelection1);
        valuesSelectionB.push_back(normalizedValueSelection2);
        loggedValuesSelectionA.push_back(std::log(normalizedValueSelection1));
        loggedValuesSelectionB.push_back(std::log(normalizedValueSelection2));

        //update temp variables to compute stats (non normalized values)
        nonNormalizedvaluesA.append(valueSelection1);
        nonNormalizedvaluesB.append(valueSelection2);
    }
    //enable sorting to the table (must be done after population)
    m_ui->tableWidget->setSortingEnabled(true);
    m_ui->tableWidget->update();

    //update plot data
    m_customPlot->graph(0)->setData(valuesSelectionA, valuesSelectionB);
    m_customPlot->graph(0)->rescaleAxes();
    m_customPlot->xAxis->setLabel(selObjectA.name());
    m_customPlot->yAxis->setLabel(selObjectB.name());
    m_customPlot->replot();

    //update UI fields for stats
    m_ui->numGenesSelectionA->setText(QString::number(countA + countAB));
    m_ui->numGenesSelectionB->setText(QString::number(countB + countAB));
    m_ui->meanSelectionA->setText(QString::number(STMath::mean(nonNormalizedvaluesA)));
    m_ui->meanSelectionB->setText(QString::number(STMath::mean(nonNormalizedvaluesB)));
    m_ui->stdDevSelectionA->setText(QString::number(STMath::std_dev(nonNormalizedvaluesA)));
    m_ui->stdDevSelectionB->setText(QString::number(STMath::std_dev(nonNormalizedvaluesB)));
    m_ui->correlation->setText(QString::number(STMath::pearson(loggedValuesSelectionA,
                                                               loggedValuesSelectionB)));
    m_ui->overlappingGenes->setText(QString::number(countAB));
    m_ui->genesOnlyA->setText(QString::number(countA));
    m_ui->genesOnlyB->setText(QString::number(countB));
    m_ui->selectionA->setText(selObjectA.name());
    m_ui->selectionB->setText(selObjectB.name());
}

void AnalysisDEA::saveToPDF()
{
    QString filename =
            QFileDialog::getSaveFileName(this, tr("Export File"), QDir::homePath(),
                                         QString("%1").arg(tr("PNG Files (*.png)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    //TODO add most DEA genes and stats (use QPrinter)
    //TODO use PDF as output
    const bool saveOk = m_customPlot->savePng(filename, 800, 800, 1.0, 100);

    if (!saveOk) {
        QMessageBox::critical(this, tr("Save DEA"), tr("Error saving DEA to a file"));
    } else {
        QMessageBox::information(this, tr("Save DEA"), tr("DEA was saved successfully"));
    }
}
