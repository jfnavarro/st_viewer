#ifndef ANALYSISCORRELATION_H
#define ANALYSISCORRELATION_H

#include <QWidget>
#include <QScatterSeries>

#include "data/STData.h"

namespace Ui {
class analysisCorrelation;
}

QT_CHARTS_USE_NAMESPACE

// This Widget takes two datasets and computes a correlation value for the common genes.
// A correlation plot is generated where users can click a dot to see which gene is it.
// It allows to normalize using the log scale
class AnalysisCorrelation : public QWidget
{
    Q_OBJECT

public:

    AnalysisCorrelation(const STData::STDataFrame &data1,
                        const STData::STDataFrame &data2,
                        const QString &nameA,
                        const QString &nameB,
                        QWidget *parent = nullptr);
    virtual ~AnalysisCorrelation();

private slots:

    // computes the correlation, updates the plot and the fields
    void slotUpdateData();

    // when the user wants to export the plot to a file
    void slotExportPlot();

    // when the user clicks a point in the plot so the gene of the point (if any) is shown
    void slotClickedPoint(const QPointF point);

private:

    // the two datasets and their names
    STData::STDataFrame m_dataA;
    STData::STDataFrame m_dataB;
    QString m_nameA;
    QString m_nameB;

    // GUI object
    QScopedPointer<Ui::analysisCorrelation> m_ui;

    // store the plotting series to allow interaction with the plot
    QScopedPointer<QScatterSeries> m_series;

    Q_DISABLE_COPY(AnalysisCorrelation)
};

#endif // ANALYSISCORRELATION_H
