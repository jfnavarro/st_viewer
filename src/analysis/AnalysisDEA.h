#ifndef ANALYSISDEA_H
#define ANALYSISDEA_H

#include <QDialog>
#include <QModelIndex>

#include "dataModel/UserSelection.h"
#include <memory>

namespace Ui
{
class ddaWidget;
}

class QTableWidget;
class GeneSelectionDEAItemModel;
class QSortFilterProxyModel;

// AnalysisDEA is a widget that contains methods to compute
// DEA(Differential Expression Analysis) between two user selections
// and show the results in a correlation plot and a table differently expressed genes
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
        // vector of counts for each selection
        std::vector<double> valuesSelectionA;
        std::vector<double> valuesSelectionB;
        // number of genes only in A
        unsigned countA;
        // number of genes only in B
        unsigned countB;
        // number of genes in both A and B
        unsigned countAB;
        // correlation value
        double pearsonCorrelation;
    };

    // Data container the counts in both selection of a gene
    // the idea is that each record contains the information of
    // one gene and its expression levels in the two selections
    struct deaReads {
        deaReads()
            : gene()
            , readsA(0)
            , readsB(0)
        {
        }

        QString gene;
        unsigned readsA;
        unsigned readsB;
    };

    typedef std::vector<deaReads> combinedSelectionType;

    AnalysisDEA(const UserSelection &selObjectA,
                const UserSelection &selObjectB,
                QWidget *parent = 0,
                Qt::WindowFlags f = 0);
    virtual ~AnalysisDEA();

    // Computes the statistics and visualization data points
    const deaStats computeStatistics();

    // Update UI elements for the statistics and correlation plots
    void updateStatisticsUI(const deaStats &stats);

signals:

private slots:

    // Threshold slider slots for number of reads (update the UI too)
    void slotSetLowerThreshold(const unsigned value);
    void slotSetUpperThreshold(const unsigned value);

    // Save correlation plot to a file
    void slotSaveToPDF();

    // To be invoked if the user selects a gene in the table
    // this will trigger a highlight of the gene in the scatter plot
    void slotSelectionSelected(QModelIndex index);

private:
    // Helper functions to get the model from the gene selections table
    GeneSelectionDEAItemModel *selectionsModel();
    QSortFilterProxyModel *selectionsProxyModel();

    // Helper function to test whether two selections are outside threshold
    // returns true if they are outside
    bool combinedSelectionThreholsd(const deaReads &deaReads) const;

    // Fills a list of deaReads objects for each shared gene in
    // both selections and also unique genes.
    // The idea is to have a structure that has information for the intersected
    // set of unique genes in both selections.
    // The computed vector is stored for convenience so it can be used
    // to compute statistics with computeStatistics()
    void computeGeneToReads(const UserSelection &selObjectA, const UserSelection &selObjectB);

    // The GUI object
    QScopedPointer<Ui::ddaWidget> m_ui;
    // We use these variables to cache the statistics for convenience
    combinedSelectionType m_combinedSelections;
    unsigned m_lowerThreshold;
    unsigned m_upperThreshold;

    Q_DISABLE_COPY(AnalysisDEA)
};

#endif // ANALYSISDEA_H
