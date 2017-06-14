#ifndef ANALYSISCLUSTERING_H
#define ANALYSISCLUSTERING_H

#include <QDialog>
#include "data/STData.h"

namespace Ui {
class analysisClustering;
}

class AnalysisClustering : public QWidget
{
    Q_OBJECT

public:
    explicit AnalysisClustering(const STData::STDataFrame &data,
                                QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~AnalysisClustering();

    QList<int> getComputedClasses() const;


public slots:

signals:

    void singalClusteringUpdated();

private slots:

    void run();

private:

    const STData::STDataFrame &m_data;

    QScopedPointer<Ui::analysisClustering> m_ui;
};

#endif // ANALYSISCLUSTERING_H
