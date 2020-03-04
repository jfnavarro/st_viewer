#ifndef ANALYSISQC_H
#define ANALYSISQC_H

#include <QWidget>

#include "data/STData.h"

namespace Ui
{
class analysisQC;
}

// AnalysisQC is a Widget that is used to show to the users
// the spots/genes distributions of a dataset
class AnalysisQC : public QWidget
{
    Q_OBJECT

public:
    explicit AnalysisQC(const STData::STDataFrame &data,
                        QWidget *parent = nullptr,
                        Qt::WindowFlags f = 0);
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
