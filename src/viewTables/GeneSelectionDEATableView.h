#ifndef GENESELECTIONDEATABLEVIEW_H
#define GENESELECTIONDEATABLEVIEW_H

#include <QTableView>
#include <QPointer>

class SortGenesProxyModel;
class GeneSelectionDEAItemModel;

// An abstraction of QTableView for the gene selections DEA table
class GeneSelectionDEATableView : public QTableView
{
    Q_OBJECT

public:
    explicit GeneSelectionDEATableView(QWidget *parent = 0);
    virtual ~GeneSelectionDEATableView();

    // returns the current selection mapped to the sorting model
    QItemSelection geneTableItemSelection() const;

public slots:

    // slot used to set a search filter for the table
    void setGeneNameFilter(const QString &str);

private:
    // references to model and proxy model
    QScopedPointer<GeneSelectionDEAItemModel> m_geneSelectionDEAModel;
    QScopedPointer<SortGenesProxyModel> m_sortGenesProxyModel;

    Q_DISABLE_COPY(GeneSelectionDEATableView)
};

#endif // GENESELECTIONDEATABLEVIEW_H
