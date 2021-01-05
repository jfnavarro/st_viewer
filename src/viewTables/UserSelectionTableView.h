#ifndef USERSELECTIONSTABLEVIEW_H
#define USERSELECTIONSTABLEVIEW_H

#include <QTableView>
#include <QPointer>

class QSortFilterProxyModel;

// An abstraction of QTableView for the user selections
class UserSelectionTableView : public QTableView
{
    Q_OBJECT

public:

    explicit UserSelectionTableView(QWidget *parent = nullptr);
    virtual ~UserSelectionTableView();

    // returns the current selection mapped to the sorting model
    QItemSelection userSelecionTableItemSelection() const;

signals:

    // signals emitted when the user eport/edit/delete a selection
    void signalExport(QModelIndex);
    void signalEdit(QModelIndex);
    void signalDelete(QModelIndex);

private slots:

    // when the user right clicks
    void customMenuRequested(const QPoint &pos);

private:

    // reference to the proxy model
    QScopedPointer<QSortFilterProxyModel> m_sortSelectionsProxyModel;

    Q_DISABLE_COPY(UserSelectionTableView)
};

#endif // USERSELECTIONSTABLEVIEW_H
