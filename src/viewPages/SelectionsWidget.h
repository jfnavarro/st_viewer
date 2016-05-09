#ifndef SELECTIONSWIDGET_H
#define SELECTIONSWIDGET_H

#include <QDockWidget>
#include <QPointer>

#include "dataModel/UserSelection.h"

class QLineEdit;
class GeneSelectionTableView;
class GeneSelectionItemModel;
class QSortFilterProxyModel;

// This widgets is part of the UserSelectionsPage. It shows the list of unique
// genes present in a UserSelection and their aggregated counts.
// It contains a search field to search genes by name.
class SelectionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SelectionsWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~SelectionsWidget();

    // clear focus/status
    void clear();

public slots:

    // reload the user selection's data model
    void slotLoadModel(const UserSelection::geneTotalCountsVector &userSelection);

signals:

private:
    // internal functions to obtain the model and the proxy model of the table
    GeneSelectionItemModel *getModel();
    QSortFilterProxyModel *getProxyModel();

    // some references needed to UI elements
    QScopedPointer<QLineEdit> m_geneSelectionFilterLineEdit;
    QScopedPointer<GeneSelectionTableView> m_selections_tableview;

    Q_DISABLE_COPY(SelectionsWidget)
};

#endif // SELECTIONSWIDGET_H
