#ifndef ANALYSISSCATTER_H
#define ANALYSISSCATTER_H

#include <QWidget>

#include "data/STData.h"

namespace Ui
{
class analysisScatter;
}

// This widget takes a data frame and shows two
// heatmap scatter plots, one with the accumulate
// counts for each spot and another one with the accumulate
// genes for each spot
class AnalysisScatter : public QWidget
{
    Q_OBJECT

public:

    AnalysisScatter(const STData::STDataFrame &data,
                    QWidget *parent = nullptr,
                    Qt::WindowFlags f = 0);

    virtual ~AnalysisScatter();

signals:

public slots:

private slots:

     // to export the scatter plots to  a file
    void slotExportPlotGenes();
    void slotExportPlotReads();

private:

    // GUI object
    QScopedPointer<Ui::analysisScatter> m_ui;

    Q_DISABLE_COPY(AnalysisScatter)
};

#endif // ANALYSISSCATTER_H
