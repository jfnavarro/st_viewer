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



signals:

private slots:

    void exportTable();

private:

    void run();
    void updateTable();
    void updatePlot();

    // GUI object
    QScopedPointer<Ui::analysisDEA> m_ui;

    // the two datasets
    mat m_dataA;
    mat m_dataB;
    std::vector<std::string> m_rowsA;
    std::vector<std::string> m_rowsB;
    std::vector<std::string> m_colsA;
    std::vector<std::string> m_colsB;

    // cache the results to not recompute
    mat m_results;
    std::vector<std::string> m_results_cols;
    std::vector<std::string> m_results_rows;
    bool m_initialized;
    SettingsWidget::NormalizationMode m_normalization;

    Q_DISABLE_COPY(AnalysisDEA)
};

#endif // ANALYSISDEA_H
