#ifndef USERSELECTIONSTABLEVIEW_H
#define USERSELECTIONSTABLEVIEW_H

#include <QTableView>
#include <QPointer>

class QSortFilterProxyModel;

// An abstraction of QTableView for the user selections page's table
class UserSelectionTableView : public QTableView
{
    Q_OBJECT

public:

    explicit UserSelectionTableView(QWidget *parent = 0);
    virtual ~UserSelectionTableView();

    // returns the current selection mapped to the sorting model
    QItemSelection userSelecionTableItemSelection() const;

signals:

    void signalSelectionExport(QModelIndex);
    void signalSelectionEdit(QModelIndex);
    void signalSelectionDelete(QModelIndex);

private slots:

    // when the user right clicks
    void customMenuRequested(const QPoint &pos);

private:
    // references to the proxy model
    QScopedPointer<QSortFilterProxyModel> m_sortSelectionsProxyModel;

    Q_DISABLE_COPY(UserSelectionTableView)
};

#endif // USERSELECTIONSTABLEVIEW_H
