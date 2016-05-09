#ifndef GENESTABLEVIEW_H
#define GENESTABLEVIEW_H

#include <QTableView>
#include <QPointer>

class GeneFeatureItemModel;
class SortGenesProxyModel;

// An abstraction of QTableView for the genes table
class GenesTableView : public QTableView
{
    Q_OBJECT

public:
    explicit GenesTableView(QWidget *parent = 0);
    virtual ~GenesTableView();

    // returns the current selection mapped to the sorting model
    QItemSelection geneTableItemSelection() const;

public slots:

    // slot used to set a search filter for the table
    void setGeneNameFilter(const QString &str);

private:
    // references to model and proxy model
    QScopedPointer<GeneFeatureItemModel> m_geneModel;
    QScopedPointer<SortGenesProxyModel> m_sortGenesProxyModel;

    Q_DISABLE_COPY(GenesTableView)
};

#endif // GENESTABLEVIEW_H
