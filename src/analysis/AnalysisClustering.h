#ifndef ANALYSISCLUSTERING_H
#define ANALYSISCLUSTERING_H

#include <QDialog>

namespace Ui {
class analysisClustering;
}

class AnalysisClustering : public QDialog
{
    Q_OBJECT

public:
    explicit AnalysisClustering(QWidget *parent = 0);
    ~AnalysisClustering();

private:
    Ui::analysisClustering *ui;
};

#endif // ANALYSISCLUSTERING_H
