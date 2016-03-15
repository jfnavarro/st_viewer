#ifndef ANALYSISDEA_H
#define ANALYSISDEA_H

#include "dataModel/UserSelection.h"
#include <memory>
#include <QDialog>
#include <QModelIndex>

namespace Ui
{
class ddaWidget;
}

class QTableWidget;
class GeneSelectionDEAItemModel;
class QSortFilterProxyModel;

// AnalysisDEA is a widget that contains methods to compute
// the DEA statistics from two user selections and show them to the user with a correlation plot
// and a table
// TODO perhaps we should separate the visualization and the computation logic
class AnalysisDEA : public QDialog
{
    Q_OBJECT

public:
    // Data container for the computed statistics
    struct deaStats {

        deaStats()
            : countA(0)
            , countB(0)
            , countAB(0)
            , pearsonCorrelation(0.0)
        {
        }

        QVector<qreal> valuesSelectionA;
        QVector<qreal> valuesSelectionB;
        unsigned countA;
        unsigned countB;
        unsigned countAB;
        qreal pearsonCorrelation;
    };

    // Data container for normalized and unnormalized reads
    // the idea is that each record contains the information of
    // one gene and its expression levels in two selections
    struct deaReads {
        deaReads()
            : gene()
            , readsA(0)
            , readsB(0)
            , normalizedReadsA(1.0)
            , normalizedReadsB(1.0)
        {
        }

        QString gene;
        unsigned readsA;
        unsigned readsB;
        qreal normalizedReadsA;
        qreal normalizedReadsB;
    };

    typedef QList<deaReads> combinedSelectionsType;

    AnalysisDEA(const UserSelection& selObjectA,
                const UserSelection& selObjectB,
                QWidget* parent = 0,
                Qt::WindowFlags f = 0);
    virtual ~AnalysisDEA();

    // Computes the statistics and visualization data points
    const deaStats computeStatistics();

    // Update UI elements for the statistics and correlation plots
    void updateStatisticsUI(const deaStats& stats);

signals:

private slots:

    // Threshold slider slots (update the UI too)
    void slotSetLowerThreshold(const unsigned value);
    void slotSetUpperThreshold(const unsigned value);
    void slotSetLowerTPMsThreshold(const unsigned value);
    void slotSetUpperTPMsThreshold(const unsigned value);

    // Save correlation plot to a file
    void slotSaveToPDF();

    // To be invoked if the user selects a gene in the table
    // this will trigger a highlight of the gene in the scatter plot
    void slotSelectionSelected(QModelIndex index);

private:
    // Helper functions to get the model from the gene selections table
    GeneSelectionDEAItemModel* selectionsModel();
    QSortFilterProxyModel* selectionsProxyModel();

    // Helper function to test whether two selections are outside threshold
    // returns true if they are outside
    bool combinedSelectionThreholsd(const deaReads& deaReads) const;

    // Fills a list of combinedSelectionsType objects for each shared gene in
    // both selections and also unique genes.
    // The idea is to have a structure that has information for the intersected
    // set of unique genes in both selections.
    // The map is stored in the class and it is used to compute statistics
    void computeGeneToReads(const UserSelection& selObjectA, const UserSelection& selObjectB);

    std::unique_ptr<Ui::ddaWidget> m_ui;
    // We use these variables to cache the statistics for convenience
    combinedSelectionsType m_combinedSelections;
    unsigned m_lowerThreshold;
    unsigned m_upperThreshold;
    unsigned m_lowerTPMsThreshold;
    unsigned m_upperTPMsThreshold;
    unsigned m_totalReadsSelA;
    unsigned m_totalReadsSelB;

    Q_DISABLE_COPY(AnalysisDEA)
};

#endif // ANALYSISDEA_H
