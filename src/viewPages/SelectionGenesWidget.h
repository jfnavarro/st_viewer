#ifndef SELECTIONGENESWIDGET_H
#define SELECTIONGENESWIDGET_H

#include <QDockWidget>
#include <QPointer>
#include <data/STData.h>

class QLineEdit;
class GeneSelectionTableView;
class GeneSelectionItemModel;
class QSortFilterProxyModel;

// This widgets is part of the UserSelectionsPage. It shows the list of unique
// genes present in a UserSelection and their aggregated counts.
// It contains a search field to search genes by name.
class SelectionGenesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SelectionGenesWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~SelectionGenesWidget();

    // clear focus/status
    void clear();

public slots:

    // reload the user selection's data model
    void slotLoadModel(const STData::gene_count_list &gene_counts);

signals:

private:

    // some references needed to UI elements
    QScopedPointer<QLineEdit> m_geneSelectionFilterLineEdit;
    QScopedPointer<GeneSelectionTableView> m_selections_tableview;

    Q_DISABLE_COPY(SelectionGenesWidget)
};

#endif // SELECTIONGENESWIDGET_H
