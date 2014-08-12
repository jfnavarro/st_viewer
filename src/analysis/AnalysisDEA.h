#ifndef ANALYSISDEA_H
#define ANALYSISDEA_H

#include "qcustomplot/qcustomplot.h"
#include "dataModel/GeneSelection.h"

namespace Ui {
class ddaWidget;
}

class QTableWidget;

//This class owns a widget to show the DEA info
//it also computes the DDA using member variables
//so the same object can be used many times

//TODO move the computation of DDA from this class
//TODO no need to have all those member variables
class AnalysisDEA : public QWidget
{
    Q_OBJECT

public:

    explicit AnalysisDEA(QWidget *parent = 0);
    ~AnalysisDEA();

    void compute(const GeneSelection::selectedItemsList& selA,
                 const GeneSelection::selectedItemsList& selB,
                 const QString &selAName, const QString &selBName);
    void plot();

signals:

protected slots:

    void saveToPDF();

private:

    Ui::ddaWidget *m_ui;
    QCustomPlot *m_customPlot;
    QVector<qreal> m_valuesSelectionA;
    QVector<qreal> m_valuesSelectionB;
    QVector<qreal> m_loggedValuesSelectionA;
    QVector<qreal> m_loggedValuesSelectionB;
    qreal m_meanSelectionA;
    qreal m_meanSelectionB;
    qreal m_stdDevSelectionA;
    qreal m_stdDevSelectionB;
    qreal m_correlation;
    QString m_selectionA;
    QString m_selectionB;
    int m_countAB;
    int m_countA;
    int m_countB;
};

#endif // ANALYSISDEA_H
