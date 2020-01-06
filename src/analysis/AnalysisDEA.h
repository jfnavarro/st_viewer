#ifndef ANALYSISDEA_H
#define ANALYSISDEA_H

#include <QWidget>
#include <QModelIndex>
#include <QSortFilterProxyModel>
#include <QFutureWatcher>

#include <string>

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

    struct DEResult {
        double pvalue;
        double log_pvalue;
        double fdr;
        double log2fc;
        QString gene;
    };

public:

    AnalysisDEA(const STData::STDataFrame &datasetsA,
                const STData::STDataFrame &datasetsB,
                const QString &nameA,
                const QString &nameB,
                QWidget *parent = nullptr,
                Qt::WindowFlags f = 0);
    virtual ~AnalysisDEA();



signals:

private slots:

    // the user wants to export the DE genes
    void slotExportTable();
    // the user has selected a DE gene in the table
    void slotGeneSelected(QModelIndex index);
    // when the DE genes have been computed in the worker thread
    void slotDEAComputed();
    // to export the volcano plot to  a file
    void slotExportPlot();
    // to handle when the user right clicks
    void customMenuRequested(const QPoint &pos);
    // to initialize the data (DE genes and volcano plot)
    void slotRun();

private:

    void runDEA(const mat &A, const mat &B, const QList<QString> genes);
    void updateTable();
    void updatePlot();

    // GUI object
    QScopedPointer<Ui::analysisDEA> m_ui;

    // the merged data frame and the selections names
    STData::STDataFrame m_dataA;
    STData::STDataFrame m_dataB;

    // cache the settings to not recompute always
    int m_reads_threshold;
    int m_genes_threshold;
    int m_spots_threshold;

    // cache the results to not recompute
    std::vector<DEResult> m_results;

    // the gene to highlight in the volcano plot
    QPointF m_gene_highlight;

    // the proxy model
    QScopedPointer<QSortFilterProxyModel> m_proxy;

    // The computational thread
    QFutureWatcher<void> m_watcher;

    Q_DISABLE_COPY(AnalysisDEA)
};

#endif // ANALYSISDEA_H
