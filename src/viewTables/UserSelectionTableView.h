#ifndef USERSELECTIONSTABLEVIEW_H
#define USERSELECTIONSTABLEVIEW_H

#include <QTableView>
#include <QPointer>

class QSortFilterProxyModel;

// An abstraction of QTableView for the user selections page's table
class UserSelectionTableView : public QTableView
{

public:
    explicit UserSelectionTableView(QWidget *parent = 0);
    virtual ~UserSelectionTableView();

    // returns the current selection mapped to the sorting model
    QItemSelection userSelecionTableItemSelection() const;

private:
    // references to the proxy model
    QScopedPointer<QSortFilterProxyModel> m_sortSelectionsProxyModel;

    Q_DISABLE_COPY(UserSelectionTableView)
};

#endif // USERSELECTIONSTABLEVIEW_H
