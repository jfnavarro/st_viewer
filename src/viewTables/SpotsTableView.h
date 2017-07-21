#ifndef SPOTSTABLEVIEW_H
#define SPOTSTABLEVIEW_H

#include <QTableView>
#include <QPointer>

class QSortFilterProxyModel;
class SpotItemModel;

// An abstraction of QTableView for the spots table
class SpotsTableView : public QTableView
{
    Q_OBJECT

public:
    explicit SpotsTableView(QWidget *parent = 0);
    virtual ~SpotsTableView();

    // returns the current selection mapped to the sorting model
    QItemSelection getItemSelection() const;

    //  Functions to retrieve the model and the proxy model of the table
    QSortFilterProxyModel *getProxyModel();
    SpotItemModel *getModel();

signals:

    // signals emitted when the user selects or change colors of spots
    void signalSpotsUpdated();

public slots:

    // slot used to set a search on the table by name
    void setNameFilter(const QString &str);

private slots:

    // slot to handle when the user right clicks
    void customMenuRequested(const QPoint &pos);

private:
    // references to  the proxy model
    QScopedPointer<QSortFilterProxyModel> m_sortProxyModel;

    Q_DISABLE_COPY(SpotsTableView)

};

#endif // SPOTSTABLEVIEW_H
