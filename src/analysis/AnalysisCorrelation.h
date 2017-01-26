#ifndef ANALYSISCORRELATION_H
#define ANALYSISCORRELATION_H

#include <QDialog>

namespace Ui {
class AnalysisCorrelation;
}

class AnalysisCorrelation : public QDialog
{
    Q_OBJECT

public:
    explicit AnalysisCorrelation(QWidget *parent = 0);
    ~AnalysisCorrelation();

private:
    Ui::AnalysisCorrelation *ui;
};

#endif // ANALYSISCORRELATION_H
