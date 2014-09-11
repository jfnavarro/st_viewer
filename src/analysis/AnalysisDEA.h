#ifndef ANALYSISDEA_H
#define ANALYSISDEA_H

#include <memory>

#include "qcustomplot/qcustomplot.h"
#include "dataModel/GeneSelection.h"

namespace Ui {
class ddaWidget;
}

class QTableWidget;

//TODO move a separate class
//Wrapper around QTableWidgetItem to allow
//to add integer elements to the table
class TableItem : public QTableWidgetItem
{
public:
    TableItem(const QString & text) : QTableWidgetItem(text)
    {}

    TableItem(int num) : QTableWidgetItem(QString::number(num))
    {}

    bool operator< (const QTableWidgetItem &other) const
    {
        if (other.column() == 1 || other.column() == 2) {
            // Compare cell data as integers for the second and third column.
            return text().toInt() < other.text().toInt();
        }
        return other.text() < text();
    }
};

//This class owns a widget to show the DEA info
//it also computes the DDA using member variables
//so the same object can be used many times

//TODO move the computation of DDA from this class
//TODO no need to have all those member variables
class AnalysisDEA : public QDialog
{
    Q_OBJECT

public:

    explicit AnalysisDEA(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~AnalysisDEA();

    void compute(const GeneSelection& selObjectA,
                 const GeneSelection& selObjectB);
    void plot();

signals:

protected slots:

    void saveToPDF();

private:

    std::unique_ptr<Ui::ddaWidget> m_ui;
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
