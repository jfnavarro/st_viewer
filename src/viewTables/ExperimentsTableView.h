#ifndef EXPERIMENTSTABLEVIEW_H
#define EXPERIMENTSTABLEVIEW_H

#include <QTableView>
#include <QPointer>

class ExperimentsItemModel;
class QSortFilterProxyModel;

class ExperimentsTableView : public QTableView
{

public:

    explicit ExperimentsTableView(QWidget *parent = 0);
    virtual ~ExperimentsTableView();

    // returns the current selection mapped to the sorting model
    QItemSelection experimentTableItemSelection() const;

private:
    //references to model and proxy model
    QPointer<ExperimentsItemModel> m_experimentModel;
    QPointer<QSortFilterProxyModel> m_sortSelectionsProxyModel;

    Q_DISABLE_COPY(ExperimentsTableView)
};

#endif // EXPERIMENTSTABLEVIEW_H
