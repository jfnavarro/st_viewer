#ifndef EXPERIMENTSTABLEVIEW_H
#define EXPERIMENTSTABLEVIEW_H

#include <QTableView>

class ExperimentsItemModel;

class ExperimentsTableView : public QTableView
{
public:
    explicit ExperimentsTableView(QWidget *parent = 0);
    virtual ~ExperimentsTableView();

private:
    ExperimentsItemModel *m_experimentModel;

    Q_DISABLE_COPY(ExperimentsTableView)
};

#endif // EXPERIMENTSTABLEVIEW_H
