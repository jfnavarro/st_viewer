#ifndef DATASETSTABLEVIEW_H
#define DATASETSTABLEVIEW_H

#include <QTableView>
#include <QPointer>

class QSortFilterProxyModel;

// An abstraction of QTableView for the datasets
class DatasetsTableView : public QTableView
{
    Q_OBJECT

public:
    explicit DatasetsTableView(QWidget *parent = nullptr);
    virtual ~DatasetsTableView();

    // returns the current selection mapped to the sorting model
    QItemSelection datasetsTableItemSelection() const;

signals:
    // signals emitted when the user open/edit/delete a dataset
    void signalOpen(QModelIndex index);
    void signalEdit(QModelIndex index);
    void signalDelete(QModelIndex index);

private slots:
    // when the user right clicks
    void customMenuRequested(const QPoint &pos);

private:
    // reference to proxy model
    QScopedPointer<QSortFilterProxyModel> m_sortDatasetsProxyModel;

    Q_DISABLE_COPY(DatasetsTableView)
};

#endif // DATASETSTABLEVIEW_H //
