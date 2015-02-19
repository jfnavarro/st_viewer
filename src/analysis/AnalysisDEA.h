/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef ANALYSISDEA_H
#define ANALYSISDEA_H

#include <memory>
#include <QDialog>
#include <QModelIndex>
#include "dataModel/GeneSelection.h"

namespace Ui {
class ddaWidget;
}

class QTableWidget;
class GeneSelectionDEAItemModel;
class QSortFilterProxyModel;

//This class owns a widget to show the DEA info
//it also computes the DDA using member variables
//so the same object can be used many times
class AnalysisDEA : public QDialog
{
    Q_OBJECT

public:

    //data container for the computed statistics
    struct deaStats {

       deaStats()
           : countA(0),
             countB(0),
             countAB(0),
             pearsonCorrelation(0.0) {}

       QVector<qreal> valuesSelectionA;
       QVector<qreal> valuesSelectionB;
       int countA;
       int countB;
       int countAB;
       qreal pearsonCorrelation;
    };

    //data container for normalized and unnormalized reads
    struct deaReads {

        deaReads()
            : gene(),
              readsA(0),
              readsB(0),
              normalizedReadsA(1.0),
              normalizedReadsB(1.0) {}

        QString gene;
        int readsA;
        int readsB;
        qreal normalizedReadsA;
        qreal normalizedReadsB;
    };

    //typedef QHash<QString, deaReads> geneToReadsPairType;
    typedef QList<deaReads> combinedSelectionsType;

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

    // threshold slider slots (update the UI too)
    void slotSetLowerThreshold(const int value);
    void slotSetUpperThreshold(const int value);
    void slotSetLowerTPMsThreshold(const int value);
    void slotSetUpperTPMsThreshold(const int value);

    // save correlation plot to a file
    void slotSaveToPDF();

    void slotSelectionSelected(QModelIndex index);

private:

    GeneSelectionDEAItemModel *selectionsModel();
    QSortFilterProxyModel *selectionsProxyModel();

    // compute the map of genes to read pairs used to
    // compute the statistics
    void computeGeneToReads(const GeneSelection& selObjectA,
                            const GeneSelection& selObjectB);

    // populate the genes expression table and update the UI (size is size of the table)
    void populateTable(const int size);

    std::unique_ptr<Ui::ddaWidget> m_ui;

    combinedSelectionsType m_combinedSelections;
    int m_lowerThreshold;
    int m_upperThreshold;
    int m_lowerTPMsThreshold;
    int m_upperTPMsThreshold;

    Q_DISABLE_COPY(AnalysisDEA)
};

#endif // ANALYSISDEA_H
