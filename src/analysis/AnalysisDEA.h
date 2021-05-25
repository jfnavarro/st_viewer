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

// A Widget that contains methods to perform a
// DEA (Differential Expression Analysis) between two datasets
// It shows the results in a volcano plot and a table
// that highlights the differentially expressed genes at a given threshold
// Users can interact with the plot and table and export both to files
// Different filtering and threshold options are provided
class AnalysisDEA : public QWidget
{
    Q_OBJECT

    // easy structure to store the DEA results
    struct DEResult {
        double pvalue;
        double log_pvalue;
        double adj_pvalue;
        double logfc;
        QString gene;
    };

public:

    AnalysisDEA(const STData::STDataFrame &datasetsA,
                const STData::STDataFrame &datasetsB,
                const QString &nameA,
                const QString &nameB,
                QWidget *parent = nullptr);

    virtual ~AnalysisDEA();

signals:

private slots:

    // when the user wants to export the DE genes
    void slotExportTable();

    // when the user has selected a DE gene in the table so it can be highlighted in the volcano plot
    void slotGeneSelected(QModelIndex index);

    // when the DE analysis has been completed so the volcano plot and the table can be updated
    void slotDEAComputed();

    // when user wants to export the volcano plot to a file
    void slotExportPlot();

    // to handle when the user right clicks in the table
    void customMenuRequested(const QPoint &pos);

    // to initialize the DE analysis
    void slotRun();

private:

    // internal functions to compute the DE genes and update the table and volcano plot when finished
    // the computation in run on a different thread
    void runDEA(QPromise<void> &promise, mat A, mat B, QList<QString> genes);
    void updateTable();
    void updatePlot();

    // GUI object
    QScopedPointer<Ui::analysisDEA> m_ui;

    // the two datasets
    STData::STDataFrame m_dataA;
    STData::STDataFrame m_dataB;

    // cache the settings to not recompute always
    int m_reads_threshold;
    int m_genes_threshold;
    int m_spots_threshold;
    SettingsWidget::NormalizationMode m_normalization;

    // cache the results to not recompute always
    QVector<DEResult> m_results;

    // the gene to highlight in the volcano plot
    QPointF m_gene_highlight;

    // the proxy model
    QScopedPointer<QSortFilterProxyModel> m_proxy;

    // The computational thread
    QFutureWatcher<void> m_watcher;

    Q_DISABLE_COPY(AnalysisDEA)
};

#endif // ANALYSISDEA_H
