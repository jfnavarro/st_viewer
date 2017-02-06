#ifndef ANALYSISCLUSTERING_H
#define ANALYSISCLUSTERING_H

#include <QDialog>

namespace Ui {
class AnalysisClustering;
}

class AnalysisClustering : public QDialog
{
    Q_OBJECT

public:
    explicit AnalysisClustering(QWidget *parent = 0);
    ~AnalysisClustering();

private:
    Ui::AnalysisClustering *ui;
};

#endif // ANALYSISCLUSTERING_H
