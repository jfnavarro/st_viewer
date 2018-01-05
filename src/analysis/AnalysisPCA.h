#ifndef ANALYSISPCA_H
#define ANALYSISPCA_H

#include <QWidget>

#include "data/STData.h"

namespace Ui {
class AnalysisPCA;
}

class AnalysisPCA : public QWidget
{
    Q_OBJECT

public:

    AnalysisPCA(const QList<STData::STDataFrame> &datasets,
                const QList<QString> &names,
                QWidget *parent = nullptr, Qt::WindowFlags f = 0);
    virtual ~AnalysisPCA();

private slots:

    // to save the plot to a file
    void slotExportPlot();

private:

    // GUI object
    QScopedPointer<Ui::AnalysisPCA> m_ui;

    Q_DISABLE_COPY(AnalysisPCA)
};

#endif // ANALYSISPCA_H
