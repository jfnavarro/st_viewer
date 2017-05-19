#ifndef SPOTSTABLEVIEW_H
#define SPOTSTABLEVIEW_H

#include <QTableView>
#include <QPointer>

class SpotItemModel;
class QSortFilterProxyModel;

// An abstraction of QTableView for the spots table
class SpotsTableView : public QTableView
{
    Q_OBJECT

public:
    explicit SpotsTableView(QWidget *parent = 0);
    virtual ~SpotsTableView();

    // returns the current selection mapped to the sorting model
    QItemSelection getItemSelection() const;

public slots:
    // slot used to set a search filter for the table
    void setNameFilter(const QString &str);

private:
    // references to model and proxy model
    QScopedPointer<SpotItemModel> m_model;
    QScopedPointer<QSortFilterProxyModel> m_sortProxyModel;

    Q_DISABLE_COPY(SpotsTableView)

};

#endif // SPOTSTABLEVIEW_H
