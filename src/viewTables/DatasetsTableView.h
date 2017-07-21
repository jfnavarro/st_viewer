#ifndef DATASETSTABLEVIEW_H
#define DATASETSTABLEVIEW_H

#include <QTableView>
#include <QPointer>

class QSortFilterProxyModel;

// An abstraction of QTableView for the datasets page's table
class DatasetsTableView : public QTableView
{
    Q_OBJECT

public:
    explicit DatasetsTableView(QWidget *parent = 0);
    virtual ~DatasetsTableView();

    // returns the current selection mapped to the sorting model
    QItemSelection datasetsTableItemSelection() const;

signals:

    void signalDatasetOpen(QModelIndex index);
    void signalDatasetEdit(QModelIndex index);
    void signalDatasetDelete(QModelIndex index);

private slots:

    // when the user right clicks
    void customMenuRequested(const QPoint &pos);

private:
    // references to proxy model
    QScopedPointer<QSortFilterProxyModel> m_sortDatasetsProxyModel;

    Q_DISABLE_COPY(DatasetsTableView)
};

#endif // DATASETSTABLEVIEW_H //
