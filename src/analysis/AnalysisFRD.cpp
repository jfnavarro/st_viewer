#include "AnalysisFRD.h"

#include "ui_frdWidget.h"

#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

#include <cmath>
#include <unordered_map>

#include "math/Common.h"

static const QColor BORDER = QColor(238, 122, 0);
static const QColor BORDER_LIGHTER = QColor(238, 122, 0, 100);

AnalysisFRD::AnalysisFRD(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , m_ui(new Ui::frdWidget)
    , m_customPlotNormal(nullptr)
    , m_customPlotLog(nullptr)
{
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    setModal(false);
    m_ui->setupUi(this);

    // TODO these init functions have common code
    initializePlotNormal();
    initializePlotLog();
}

AnalysisFRD::~AnalysisFRD()
{
}

void AnalysisFRD::initializePlotNormal()
{
    // creating plotting object for normal reads ditribution
    m_customPlotNormal = new QCustomPlot(m_ui->plotNormalWidget);
    Q_ASSERT(!m_customPlotNormal.isNull());

    // add plot of data
    m_customPlotNormal->addGraph();
    m_customPlotNormal->graph(0)->setPen(QPen(BORDER));
    m_customPlotNormal->graph(0)->setBrush(QBrush(BORDER_LIGHTER));

    // configure right and top axis to show ticks but no labels:
    m_customPlotNormal->xAxis2->setVisible(true);
    m_customPlotNormal->xAxis2->setTickLabels(false);
    m_customPlotNormal->yAxis2->setVisible(true);
    m_customPlotNormal->yAxis2->setTickLabels(false);
    m_customPlotNormal->xAxis->setLabel("Reads counts");
    m_customPlotNormal->yAxis->setLabel("# Unique genes per barcode");

    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(m_customPlotNormal->xAxis,
            SIGNAL(rangeChanged(QCPRange)),
            m_customPlotNormal->xAxis2,
            SLOT(setRange(QCPRange)));
    connect(m_customPlotNormal->yAxis,
            SIGNAL(rangeChanged(QCPRange)),
            m_customPlotNormal->yAxis2,
            SLOT(setRange(QCPRange)));

    // connect slots that takes care that when an axis is selected,
    // only that direction can be dragged and zoomed:
    connect(m_customPlotNormal.data(), SIGNAL(mousePress(QMouseEvent *)), this, SLOT(mousePress()));
    connect(m_customPlotNormal.data(), SIGNAL(mouseWheel(QWheelEvent *)), this, SLOT(mouseWheel()));

    m_customPlotNormal->setFixedSize(550, 550);
    m_customPlotNormal->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}

void AnalysisFRD::initializePlotLog()
{
    // creating plotting object for log reads ditribution
    m_customPlotLog = new QCustomPlot(m_ui->plotLogWidget);
    Q_ASSERT(!m_customPlotLog.isNull());

    // add plot of data
    m_customPlotLog->addGraph();
    m_customPlotLog->graph(0)->setPen(QPen(BORDER));
    m_customPlotLog->graph(0)->setBrush(QBrush(BORDER_LIGHTER));

    // configure right and top axis to show ticks but no labels:
    m_customPlotLog->xAxis2->setVisible(true);
    m_customPlotLog->xAxis2->setTickLabels(false);
    m_customPlotLog->yAxis2->setVisible(true);
    m_customPlotLog->yAxis2->setTickLabels(false);
    m_customPlotLog->xAxis->setLabel("Reads counts (log)");
    m_customPlotLog->yAxis->setLabel("# Unique genes per barcode (log)");
    // TODO set log scale for axes

    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(m_customPlotLog->xAxis,
            SIGNAL(rangeChanged(QCPRange)),
            m_customPlotLog->xAxis2,
            SLOT(setRange(QCPRange)));
    connect(m_customPlotLog->yAxis,
            SIGNAL(rangeChanged(QCPRange)),
            m_customPlotLog->yAxis2,
            SLOT(setRange(QCPRange)));

    // connect slots that takes care that when an axis is selected,
    // only that direction can be dragged and zoomed:
    connect(m_customPlotLog.data(), SIGNAL(mousePress(QMouseEvent *)), this, SLOT(mousePress()));
    connect(m_customPlotLog.data(), SIGNAL(mouseWheel(QWheelEvent *)), this, SLOT(mouseWheel()));

    m_customPlotLog->setFixedSize(550, 550);
    m_customPlotLog->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}

void AnalysisFRD::computeData(const STData &dataset)
{
    Q_UNUSED(dataset)
/*
    QHash<int, int> featureCounter;

    // iterate the features to compute hash tables to help to obtain the X and Y axes for the plots
    for (const auto &feature : dataset) {
        ++featureCounter[feature->count()];
    };

    // QCustomPlot only accepts QVector<double>
    QVector<double> x;
    QVector<double> y;
    QVector<double> x_log;
    QVector<double> y_log;
    // iterate hash to populate the vectors for the plot
    QHash<int, int>::const_iterator it = featureCounter.constBegin();
    while (it != featureCounter.constEnd()) {
        const double read_value = static_cast<double>(it.key());
        const double value_count = static_cast<double>(it.value());
        x.push_back(read_value);
        y.push_back(value_count);
        x_log.push_back(std::log1p(read_value));
        y_log.push_back(std::log1p(value_count));
        ++it;
    }

    // TODO probably no need to keep max values of Y as it can be obtained form
    // the plotting object
    m_maxY = *std::max_element(y.begin(), y.end());
    m_minY = *std::min_element(y.begin(), y.end());

    // update bars
    setLowerLimit(min);
    setUpperLimit(max);

    // update plot data
    m_customPlotNormal->graph(0)->setData(x, y);
    m_customPlotNormal->graph(0)->rescaleAxes();
    m_customPlotNormal->replot();
    m_customPlotLog->graph(0)->setData(x_log, y_log);
    m_customPlotLog->graph(0)->rescaleAxes();
    m_customPlotLog->replot();
*/
}

void AnalysisFRD::mousePress()
{
    // if an axis is selected, only allow the direction of that axis to be dragged
    // if no axis is selected, both directions may be dragged

    if (m_customPlotNormal->xAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
        m_customPlotNormal->axisRect()->setRangeDrag(m_customPlotNormal->xAxis->orientation());
    } else if (m_customPlotNormal->yAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
        m_customPlotNormal->axisRect()->setRangeDrag(m_customPlotNormal->yAxis->orientation());
    } else {
        m_customPlotNormal->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    }
}

void AnalysisFRD::mouseWheel()
{
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed

    if (m_customPlotNormal->xAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
        m_customPlotNormal->axisRect()->setRangeZoom(m_customPlotNormal->xAxis->orientation());
    } else if (m_customPlotNormal->yAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
        m_customPlotNormal->axisRect()->setRangeZoom(m_customPlotNormal->yAxis->orientation());
    } else {
        m_customPlotNormal->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    }
}

