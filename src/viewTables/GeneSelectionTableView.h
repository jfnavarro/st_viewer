#ifndef GENESELECTIONTABLEVIEW_H
#define GENESELECTIONTABLEVIEW_H

#include <QTableView>
#include <QPointer>

class GeneSelectionItemModel;
class SortGenesProxyModel;

// An abstraction of QTableView for the gene selections table in the user
// selections window
class GeneSelectionTableView : public QTableView
{
    Q_OBJECT

public:
    explicit GeneSelectionTableView(QWidget *parent = 0);
    virtual ~GeneSelectionTableView();

    // returns the current selection mapped to the sorting model
    QItemSelection geneTableItemSelection() const;

public slots:

    // slot used to set a search filter for the table
    void setGeneNameFilter(const QString &str);

private:
    // references to model and proxy model
    QScopedPointer<GeneSelectionItemModel> m_geneSelectionModel;
    QScopedPointer<SortGenesProxyModel> m_sortGenesProxyModel;

    Q_DISABLE_COPY(GeneSelectionTableView)
};

#endif // GENESELECTIONTABLEVIEW_H
