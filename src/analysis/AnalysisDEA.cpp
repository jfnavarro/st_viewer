#include "AnalysisDEA.h"
#include "ui_ddaWidget.h"
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

#include <cmath>
#include "math/Common.h"

AnalysisDEA::AnalysisDEA(QWidget *parent) :
    QWidget(parent),
    m_ui(nullptr),
    m_customPlot(nullptr),
    m_meanSelectionA(0.0),
    m_meanSelectionB(0.0),
    m_stdDevSelectionA(0.0),
    m_stdDevSelectionB(0.0),
    m_correlation(0.0),
    m_selectionA(),
    m_selectionB(),
    m_countAB(0),
    m_countA(0),
    m_countB(0)
{
    // create UI
    m_ui = new Ui::ddaWidget;
    m_ui->setupUi(this);

    // creating plotting object
    m_customPlot = new QCustomPlot(m_ui->plotWidget);

    //make connections
    connect(m_ui->cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(m_ui->saveButton, SIGNAL(clicked()), this, SLOT(saveToPDF()));
}

AnalysisDEA::~AnalysisDEA()
{
    if (m_ui != nullptr) {
        delete m_ui;
    }
    m_ui = nullptr;

    m_customPlot->deleteLater();
    m_customPlot = nullptr;
}

//TODO split and optimize this function
void AnalysisDEA::compute(const GeneSelection &selObjectA,
                          const GeneSelection &selObjectB)
{
    typedef QHash<QString, QPair<qreal,qreal> > geneToNormalizedPairType;

    // update the selection names
    m_selectionA = selObjectA.name();
    m_selectionB = selObjectB.name();

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
    m_ui->tableWidget->clearContents();
    // initialize columns and headers of the table
    QStringList headers;
    headers << "Gene" << "Reads Sel. A" << "Reads Sel. B";
    m_ui->tableWidget->setHorizontalHeaderLabels(headers);
    m_ui->tableWidget->setColumnCount(3);
    // initialize row size of the table
    m_ui->tableWidget->setRowCount(biggestSize);
    int index = 0; //to keep count of the elements inserted

    // some temp variables for computation of statistics (not normalized values)
    QVector<qreal> valuesA;
    QVector<qreal> valuesB;

    // reset counters for overlapping
    m_countAB = 0;
    m_countA = 0;
    m_countB = 0;

    // reset lists of values
    m_valuesSelectionA.clear();
    m_valuesSelectionB.clear();
    m_loggedValuesSelectionA.clear();
    m_loggedValuesSelectionB.clear();

    //total reads in each selection used to normalize
    const int totalReadsSelectionA = selObjectA.totalReads();
    const int totalReadsSelectionB = selObjectB.totalReads();

    //iterate the hash table to compute the DDA values
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
            m_countB++;
        } else if (valueSelection2 == 0.0) {
            m_countA++;
        } else {
            m_countAB++;
        }

        // populate lists of values with normalized values (for the scatter plot)
        // TODO validate 1.0 is a good value to assign when no gene present
        const qreal normalizedValueSelection1 = valueSelection1 != 0.0 ?
                    ((it.value().first * 10e5) / totalReadsSelectionA) + 1 : 1.0;
        const qreal normalizedValueSelection2 = valueSelection2 != 0.0 ?
                    ((it.value().second * 10e5) / totalReadsSelectionB) + 1 : 1.0;

        // update lists of values
        m_valuesSelectionA.push_back(normalizedValueSelection1);
        m_valuesSelectionB.push_back(normalizedValueSelection2);
        m_loggedValuesSelectionA.push_back(std::log10(normalizedValueSelection1));
        m_loggedValuesSelectionB.push_back(std::log10(normalizedValueSelection2));

        //update temp variables to compute stats
        valuesA.append(valueSelection1);
        valuesB.append(valueSelection2);
    }
    //enable sorting to the table (must be done after population)
    m_ui->tableWidget->setSortingEnabled(true);
    m_ui->tableWidget->update();

    //compute stats (using non normalized values)
    m_meanSelectionA = STMath::mean(valuesA);
    m_meanSelectionB = STMath::mean(valuesB);
    m_stdDevSelectionA = STMath::std_dev(valuesA);
    m_stdDevSelectionB = STMath::std_dev(valuesB);
    //for correlation use normalized logged values
    m_correlation = STMath::pearson(m_loggedValuesSelectionA, m_loggedValuesSelectionB);
}

void AnalysisDEA::plot()
{
    Q_ASSERT(m_customPlot != nullptr);

    m_customPlot->clearGraphs();
    m_customPlot->addGraph();
    m_customPlot->graph(0)->setScatterStyle(
                QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::blue), Qt::white, 5));
    m_customPlot->graph(0)->setData(m_valuesSelectionA, m_valuesSelectionB);
    m_customPlot->graph(0)->setAntialiasedScatters(true);
    m_customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
    m_customPlot->graph(0)->setName("Correlation Scatter Plot");
    m_customPlot->graph(0)->rescaleAxes();
    m_customPlot->graph(0)->rescaleAxes(true);

    // sets the legend
    m_customPlot->legend->setVisible(false);
    // give the axes some labels:
    m_customPlot->xAxis->setLabel(m_selectionA);
    m_customPlot->yAxis->setLabel(m_selectionB);
    // set axes ranges, so we see all data:
    //m_customPlot->xAxis->setRange(0, 10e4);
    //m_customPlot->yAxis->setRange(0, 10e4);
    m_customPlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
    m_customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
    m_customPlot->xAxis->setTicks(true);
    m_customPlot->yAxis->setTicks(true);
    // make top right axes clones of bottom left axes. Looks prettier:
    m_customPlot->axisRect()->setupFullAxesBox();
    // plot and add mouse interaction (fixed size)
    m_customPlot->setFixedSize(500, 400);
    m_customPlot->replot();
    m_customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    m_customPlot->show();

    //update UI fields
    m_ui->numGenesSelectionA->setText(QString::number(m_countA + m_countAB));
    m_ui->numGenesSelectionB->setText(QString::number(m_countB + m_countAB));
    m_ui->meanSelectionA->setText(QString::number(m_meanSelectionA));
    m_ui->meanSelectionB->setText(QString::number(m_meanSelectionB));
    m_ui->stdDevSelectionA->setText(QString::number(m_stdDevSelectionA));
    m_ui->stdDevSelectionB->setText(QString::number(m_stdDevSelectionB));
    m_ui->correlation->setText(QString::number(fabs(m_correlation)));
    m_ui->overlappingGenes->setText(QString::number(m_countAB));
    m_ui->genesOnlyA->setText(QString::number(m_countA));
    m_ui->genesOnlyB->setText(QString::number(m_countB));
    m_ui->selectionA->setText(m_selectionA);
    m_ui->selectionB->setText(m_selectionB);

    //show the widget
    show();
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
    // append default extension
    QRegExp regex("^.*\\.(png)$", Qt::CaseInsensitive);
    if (!regex.exactMatch(filename)) {
        filename.append(".png");
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
