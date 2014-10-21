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

struct deaStats {

   deaStats()
       : countA(0),
         countB(0),
         countAB(0),
         meanA(0.0),
         meanB(0.0),
         stdDevA(0.0),
         stdDevB(0.0),
         pearsonCorrelation(0.0) {}

   QVector<qreal> valuesSelectionA;
   QVector<qreal> valuesSelectionB;
   int countA;
   int countB;
   int countAB;
   qreal meanA;
   qreal meanB;
   qreal stdDevA;
   qreal stdDevB;
   qreal pearsonCorrelation;
};

//This class owns a widget to show the DEA info
//it also computes the DDA using member variables
//so the same object can be used many times
class AnalysisDEA : public QDialog
{
    Q_OBJECT

public:

    typedef QMap<QString, QPair<int,int> > geneToReadsPairType;

    AnalysisDEA(const GeneSelection& selObjectA,
                const GeneSelection& selObjectB,
                QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~AnalysisDEA();

    // compute the statistics and visualization data points and updates
    const deaStats computeStatistics();

    // update UI elements for the statistics and correlation plots
    void updateStatisticsUI(const deaStats& stats);

signals:

private slots:

    // threshold slider slots
    void slotSetLowerThreshold(const int value);
    void slotSetUpperThreshold(const int value);

    // save correlation plot to a file
    void slotSaveToPDF();

private:

    int computeGeneToReads(const GeneSelection& selObjectA,
                            const GeneSelection& selObjectB);

    // populate the genes expression table and update the UI
    void populateTable(const int size);

    std::unique_ptr<Ui::ddaWidget> m_ui;
    QCustomPlot *m_customPlot;
    geneToReadsPairType m_geneToReadsMap;
    int m_totalReadsSelectionA;
    int m_totalReadsSelectionB;
    int m_lowerThreshold;
    int m_upperThreshold;
};

#endif // ANALYSISDEA_H
