#ifndef GENESELECTIONTABLEVIEW_H
#define GENESELECTIONTABLEVIEW_H

#include <QTableView>

class GeneSelectionItemModel;

class GeneSelectionTableView : public QTableView
{
public:
    explicit GeneSelectionTableView(QWidget *parent = 0);
    virtual ~GeneSelectionTableView();

private:
    // mvc model

    GeneSelectionItemModel *geneSelectionModel;
};

#endif // GENESELECTIONTABLEVIEW_H
