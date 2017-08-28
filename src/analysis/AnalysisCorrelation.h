#ifndef ANALYSISCORRELATION_H
#define ANALYSISCORRELATION_H

#include <QWidget>
#include <QScatterSeries>

#include "data/STData.h"

namespace Ui {
class analysisCorrelation;
}

QT_CHARTS_USE_NAMESPACE

// This Widget takes two datasets (selections)
// and computes correlation value for the common genes
// it allows to chose normalization method and log scale and also to click and
// see the clicked gene
class AnalysisCorrelation : public QWidget
{
    Q_OBJECT

public:
    AnalysisCorrelation(const STData::STDataFrame &data1,
                        const STData::STDataFrame &data2,
                        const QString &nameA,
                        const QString &nameB,
                        QWidget *parent = 0,
                        Qt::WindowFlags f = 0);
    virtual ~AnalysisCorrelation();

private slots:

    // computes and updates the plot and correlation data
    void slotUpdateData();

    // when the user wants to export the plot to a file
    void slotExportPlot();

    // when the user clicks a point in the plot
    void slotClickedPoint(const QPointF point);

private:

    // GUI object
    QScopedPointer<Ui::analysisCorrelation> m_ui;

    // the two datasets
    STData::STDataFrame m_dataA;
    STData::STDataFrame m_dataB;
    QString m_nameA;
    QString m_nameB;

    // cache these to know what gene was clicked
    std::vector<double> m_rowsumA;
    std::vector<double> m_rowsumB;
    QList<QString> m_genes;

    // the size factors
    rowvec m_deseq_size_factorsA;
    rowvec m_deseq_size_factorsB;
    rowvec m_scran_size_factorsA;
    rowvec m_scran_size_factorsB;

    // store the plotting series to allow interaction with the plot
    QScopedPointer<QScatterSeries> m_series;

    Q_DISABLE_COPY(AnalysisCorrelation)
};

#endif // ANALYSISCORRELATION_H
