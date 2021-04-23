#ifndef ANALYSISQC_H
#define ANALYSISQC_H

#include <QWidget>

#include "data/STData.h"

namespace Ui
{
class analysisQC;
}

// Widget that is used to compute and show
// the spots/genes distributions (histograms) of a dataset and other
// useful general stats
class AnalysisQC : public QWidget
{
    Q_OBJECT

public:

    explicit AnalysisQC(const STData::STDataFrame &data,
                        QWidget *parent = nullptr);
    virtual ~AnalysisQC();

signals:

public slots:

private slots:

    // to save the histogram to a file (type 1 genes, type 2 spots)
    void slotExportPlot(const int type);

private:

    // GUI object
    QScopedPointer<Ui::analysisQC> m_ui;

    Q_DISABLE_COPY(AnalysisQC)
};
#endif // ANALYSISQC_H
