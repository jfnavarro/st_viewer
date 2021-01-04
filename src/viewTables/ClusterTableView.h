#ifndef CLUSTERTABLEVIEW_H
#define CLUSTERTABLEVIEW_H

#include <QTableView>
#include <QPointer>

class QSortFilterProxyModel;
class ClusterItemModel;

// An abstraction of QTableView for the clusters data
class ClusterTableView : public QTableView
{
    Q_OBJECT

public:
    explicit ClusterTableView(QWidget *parent = nullptr);
    virtual ~ClusterTableView();

    // returns the current selection mapped to the sorting model
    QItemSelection getItemSelection() const;

    //  Functions to retrieve the model and the proxy model of the table
    QSortFilterProxyModel *getProxyModel();
    ClusterItemModel *getModel();

signals:
    // signal emitted when the user selects or change colors of clusters
    void signalUpdated();

public slots:
    // slot used to set a search on the table by name
    void setNameFilter(const QString &str);

private slots:
    // slot to handle when the user right clicks
    void customMenuRequested(const QPoint &pos);

private:
    // reference to  the proxy model
    QScopedPointer<QSortFilterProxyModel> m_sortProxyModel;

    Q_DISABLE_COPY(ClusterTableView)
};

#endif // CLUSTERTABLEVIEW_H
