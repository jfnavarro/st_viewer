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

AnalysisDEA::AnalysisDEA(const GeneSelection& selObjectA,
                         const GeneSelection& selObjectB,
                         QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    m_ui(new Ui::ddaWidget),
    m_customPlot(nullptr),
    m_totalReadsSelectionA(0),
    m_totalReadsSelectionB(0),
    m_lowerThreshold(0),
    m_upperThreshold(1)
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

    // total reads in each selection used to normalize
    m_totalReadsSelectionA = selObjectA.totalReads();
    m_totalReadsSelectionB = selObjectB.totalReads();

    // populate the gene to read pairs containers
    const int biggestSize = computeGeneToReads(selObjectA, selObjectB);
    // computeGeneToReads will update the min and max thresholds (to initialize slider)

    m_ui->tpmThreshold->setMinimumValue(m_lowerThreshold);
    m_ui->tpmThreshold->setMaximumValue(m_upperThreshold);
    m_ui->tpmThreshold->setLowerValue(m_lowerThreshold);
    m_ui->tpmThreshold->setUpperValue(m_upperThreshold);
    m_ui->tpmThreshold->setTickInterval(1);

    // populate table
    populateTable(biggestSize);

    // update name fields
    m_ui->selectionA->setText(selObjectA.name());
    m_ui->selectionB->setText(selObjectB.name());
    m_customPlot->xAxis->setLabel(selObjectA.name());
    m_customPlot->yAxis->setLabel(selObjectB.name());

    // compute statistics
    const deaStats &stats = computeStatistics();

    // visualize statistics
    updateStatisticsUI(stats);

    // make connections
    connect(m_ui->cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(m_ui->saveButton, SIGNAL(clicked()), this, SLOT(slotSaveToPDF()));
    connect(m_ui->tpmThreshold, SIGNAL(lowerValueChanged(int)),
            this, SLOT(slotSetLowerThreshold(int)));
    connect(m_ui->tpmThreshold, SIGNAL(upperValueChanged(int)),
            this, SLOT(slotSetUpperThreshold(int)));
}

AnalysisDEA::~AnalysisDEA()
{
    m_customPlot->deleteLater();
    m_customPlot = nullptr;
}

int AnalysisDEA::computeGeneToReads(const GeneSelection& selObjectA,
                                     const GeneSelection& selObjectB)
{
    // reset threshold
    m_lowerThreshold = 10e5;
    m_upperThreshold = -1;

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
    m_geneToReadsMap.clear();
    for (int i = 0; i < biggestSize; ++i) {
        if (selection1Size > i) {
            const auto& selection1 = selA.at(i);
            m_geneToReadsMap[selection1.name].first = selection1.reads;
            //to use TPM values in threshold multiply by 10e5
            const int tpmReads = selection1.reads;
            m_lowerThreshold = std::min(tpmReads, m_lowerThreshold);
            m_upperThreshold = std::max(tpmReads, m_upperThreshold);
        }
        if (selection2Size > i) {
            const auto& selection2 = selB.at(i);
            m_geneToReadsMap[selection2.name].second = selection2.reads;
            //to use TPM values in threshold multiply by 10e5
            const int tpmReads = selection2.reads;
            m_lowerThreshold = std::min(tpmReads, m_lowerThreshold);
            m_upperThreshold = std::max(tpmReads, m_upperThreshold);
        }
    }

    //just for convenience
    return biggestSize;
}

void AnalysisDEA::populateTable(const int size)
{
    // clear the table
    m_ui->tableWidget->clear();
    // initialize columns and headers of the table
    m_ui->tableWidget->setColumnCount(3);
    QStringList headers;
    headers << "Gene" << "Reads Sel. A" << "Reads Sel. B";
    m_ui->tableWidget->setHorizontalHeaderLabels(headers);
    // initialize row size of the table
    m_ui->tableWidget->setRowCount(size);

    // iterate container to populate table
    int index = 0; //to keep count of the elements inserted
    geneToReadsPairType::const_iterator end = m_geneToReadsMap.end();
    for (geneToReadsPairType::const_iterator it = m_geneToReadsMap.begin();
         it != end; ++it) {

        const int valueSelection1 = it.value().first;
        const int valueSelection2 = it.value().second;

        // update table
        m_ui->tableWidget->setItem(index, 0, new TableItem(it.key()));
        m_ui->tableWidget->setItem(index, 1, new TableItem(valueSelection1));
        m_ui->tableWidget->setItem(index, 2, new TableItem(valueSelection2));
        index++;
    }

    //enable sorting to the table (must be done after population)
    m_ui->tableWidget->setSortingEnabled(true);
    m_ui->tableWidget->update();
}

const deaStats AnalysisDEA::computeStatistics()
{
    deaStats stats;

    if (m_geneToReadsMap.empty()) {
        return stats;
    }

    // some temp containers
    QVector<qreal> nonNormalizedvaluesA;
    QVector<qreal> nonNormalizedvaluesB;
    QVector<qreal> loggedValuesSelectionA;
    QVector<qreal> loggedValuesSelectionB;

    //iterate the hash table to compute the DDA stats and populate the table
    geneToReadsPairType::const_iterator end = m_geneToReadsMap.end();
    for (geneToReadsPairType::const_iterator it = m_geneToReadsMap.begin();
         it != end; ++it) {

        const qreal readsSelA = static_cast<qreal>(it.value().first);
        const qreal readsSelB = static_cast<qreal>(it.value().second);
        const qreal valueSelection1 = readsSelA * 10e5;
        const qreal valueSelection2 = readsSelB * 10e5;

        //to use TPM values in threshold use valuesSection1 and valuesSection2
        if (readsSelA < m_lowerThreshold || readsSelA > m_upperThreshold
            || readsSelB < m_lowerThreshold || readsSelB > m_upperThreshold) {
            continue;
        }

        // compute overlapping counting values
        if (readsSelA == 0.0) {
            stats.countB++;
        } else if (readsSelB == 0.0) {
            stats.countA++;
        } else {
            stats.countAB++;
        }

        // populate lists of values with normalized values (for the scatter plot)
        const qreal normalizedValueSelection1 =
                (valueSelection1 / m_totalReadsSelectionA) + 1;
        const qreal normalizedValueSelection2 =
                (valueSelection2 / m_totalReadsSelectionB) + 1;

        // update lists of values
        stats.valuesSelectionA.push_back(normalizedValueSelection1);
        stats.valuesSelectionB.push_back(normalizedValueSelection2);

        //update temp variables to compute stats (non normalized values and log values)
        nonNormalizedvaluesA.append(readsSelA);
        nonNormalizedvaluesB.append(readsSelB);
        loggedValuesSelectionA.push_back(std::log(normalizedValueSelection1));
        loggedValuesSelectionB.push_back(std::log(normalizedValueSelection2));
    }

    if (!nonNormalizedvaluesA.empty() && !nonNormalizedvaluesB.empty()) {
        stats.meanA = STMath::mean(nonNormalizedvaluesA);
        stats.meanB = STMath::mean(nonNormalizedvaluesB);
        stats.stdDevA = STMath::std_dev(nonNormalizedvaluesA);
        stats.stdDevB = STMath::std_dev(nonNormalizedvaluesB);
    }

    if (!loggedValuesSelectionA.empty() && !loggedValuesSelectionB.empty()) {
        stats.pearsonCorrelation = STMath::pearson(loggedValuesSelectionA,
                                                   loggedValuesSelectionB);
    }

    return stats;
}

void AnalysisDEA::updateStatisticsUI(const deaStats &stats)
{
    //update plot data
    m_customPlot->graph(0)->setData(stats.valuesSelectionA, stats.valuesSelectionB);
    m_customPlot->graph(0)->rescaleAxes();
    m_customPlot->replot();

    //update UI fields for stats
    m_ui->numGenesSelectionA->setText(QString::number(stats.countA + stats.countAB));
    m_ui->numGenesSelectionB->setText(QString::number(stats.countB + stats.countAB));
    m_ui->meanSelectionA->setText(QString::number(stats.meanA));
    m_ui->meanSelectionB->setText(QString::number(stats.meanB));
    m_ui->stdDevSelectionA->setText(QString::number(stats.stdDevA));
    m_ui->stdDevSelectionB->setText(QString::number(stats.stdDevB));
    m_ui->correlation->setText(QString::number(stats.pearsonCorrelation));
    m_ui->overlappingGenes->setText(QString::number(stats.countAB));
    m_ui->genesOnlyA->setText(QString::number(stats.countA));
    m_ui->genesOnlyB->setText(QString::number(stats.countB));

    update();
}

void AnalysisDEA::slotSetLowerThreshold(const int value)
{
    if (value != m_lowerThreshold) {
        m_lowerThreshold = value;
        updateStatisticsUI(computeStatistics());
    }
}

void AnalysisDEA::slotSetUpperThreshold(const int value)
{
    if (value != m_upperThreshold) {
        m_upperThreshold = value;
        updateStatisticsUI(computeStatistics());
    }
}

void AnalysisDEA::slotSaveToPDF()
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
