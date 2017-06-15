#ifndef ANALYSISCORRELATION_H
#define ANALYSISCORRELATION_H

#include <QWidget>

#include "data/STData.h"

namespace Ui {
class analysisCorrelation;
}

// This Widget takes two datasets (selections)
// and computes correlation value for the common genes/
// it allows to chose normalization method and log scale
class AnalysisCorrelation : public QWidget
{
    Q_OBJECT

public:
    explicit AnalysisCorrelation(const STData::STDataFrame &data1,
                                 const STData::STDataFrame &data2,
                                 QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~AnalysisCorrelation();

private slots:

    void updateData();

private:
    // GUI object
    QScopedPointer<Ui::analysisCorrelation> m_ui;

    // the two datasets
    mat m_dataA;
    mat m_dataB;

    // store the size factors to save computational time
    rowvec m_deseq_factorsA;
    rowvec m_scran_factorsA;
    rowvec m_deseq_factorsB;
    rowvec m_scran_factorsB;

    Q_DISABLE_COPY(AnalysisCorrelation)
};

#endif // ANALYSISCORRELATION_H
