#ifndef EXPERIMENTSTABLEVIEW_H
#define EXPERIMENTSTABLEVIEW_H

#include <QTableView>
#include <QPointer>

class ExperimentsItemModel;

class ExperimentsTableView : public QTableView
{

public:

    explicit ExperimentsTableView(QWidget *parent = 0);
    virtual ~ExperimentsTableView();

private:

    QPointer<ExperimentsItemModel> m_experimentModel;

    Q_DISABLE_COPY(ExperimentsTableView)
};

#endif // EXPERIMENTSTABLEVIEW_H
