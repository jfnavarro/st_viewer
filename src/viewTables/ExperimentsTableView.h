#ifndef EXPERIMENTSTABLEVIEW_H
#define EXPERIMENTSTABLEVIEW_H

#include <QTableView>
#include <QPointer>

class ExperimentsItemModel;
class QSortFilterProxyModel;

// An abstraction of QTableView for the gene selections table
// TODO rename this to genesSelection o featuresSelection
class ExperimentsTableView : public QTableView
{

public:
    explicit ExperimentsTableView(QWidget* parent = 0);
    virtual ~ExperimentsTableView();

    // returns the current selection mapped to the sorting model
    QItemSelection experimentTableItemSelection() const;

private:
    // references to model and proxy model
    QPointer<ExperimentsItemModel> m_experimentModel;
    QPointer<QSortFilterProxyModel> m_sortSelectionsProxyModel;

    Q_DISABLE_COPY(ExperimentsTableView)
};

#endif // EXPERIMENTSTABLEVIEW_H
