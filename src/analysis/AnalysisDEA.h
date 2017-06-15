#ifndef ANALYSISDEA_H
#define ANALYSISDEA_H

#include <QWidget>

#include "data/STData.h"

namespace Ui
{
class analysisDEA;
}

// AnalysisDEA is a widget that contains methods to compute
// DEA(Differential Expression Analysis) between two ST data selections
// It shows the results in a volcano plot and a table
// that includes the differently expressed genes at a given FDR
class AnalysisDEA : public QWidget
{
    Q_OBJECT

public:

    explicit AnalysisDEA(const STData::STDataFrame &data1,
                         const STData::STDataFrame &data2,
                         QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~AnalysisDEA();

    void exportTable() const;

signals:

private slots:

private:

    void run();

    // the two datasets
    STData::STDataFrame m_dataA;
    STData::STDataFrame m_dataB;

    // GUI object
    QScopedPointer<Ui::analysisDEA> m_ui;

    // store the size factors to save computational time
    rowvec m_deseq_factorsA;
    rowvec m_scran_factorsA;
    rowvec m_deseq_factorsB;
    rowvec m_scran_factorsB;

    Q_DISABLE_COPY(AnalysisDEA)
};

#endif // ANALYSISDEA_H
