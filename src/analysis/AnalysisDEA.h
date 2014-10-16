/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

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
class AnalysisDEA : public QDialog
{
    Q_OBJECT

public:

    explicit AnalysisDEA(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~AnalysisDEA();

    // compute the statistics and visualization data points and updates
    // the visual components
    void computeData(const GeneSelection& selObjectA,
                     const GeneSelection& selObjectB);

signals:

private slots:

    void saveToPDF();

private:

    std::unique_ptr<Ui::ddaWidget> m_ui;
    QCustomPlot *m_customPlot;

};

#endif // ANALYSISDEA_H
