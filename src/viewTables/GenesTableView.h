#ifndef GENESTABLEVIEW_H
#define GENESTABLEVIEW_H

#include <QTableView>
#include <QPointer>

class QSortFilterProxyModel;
class GeneItemModel;

// An abstraction of QTableView for the genes table
class GenesTableView : public QTableView
{
    Q_OBJECT

public:
    explicit GenesTableView(QWidget *parent = 0);
    virtual ~GenesTableView();

    // returns the current selection mapped to the sorting model
    QItemSelection getItemSelection() const;

    //  Functions to retrieve the model and the proxy model of the table
    QSortFilterProxyModel *getProxyModel();
    GeneItemModel *getModel();

signals:

    // signals emitted when the user selects or change colors of genes
    void signalGenesUpdated();

public slots:

    // slot used to set a search on the table by name
    void setNameFilter(const QString &str);

private slots:

    // slot to handle when the user right clicks
    void customMenuRequested(const QPoint &pos);

private:

    // references to the proxy model
    QScopedPointer<QSortFilterProxyModel> m_sortProxyModel;

    Q_DISABLE_COPY(GenesTableView)
};

#endif // GENESTABLEVIEW_H
