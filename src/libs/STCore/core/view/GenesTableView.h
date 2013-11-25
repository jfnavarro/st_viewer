#ifndef GENESTABLEVIEW_H
#define GENESTABLEVIEW_H

#include <QTableView>

class GeneFeatureItemModel;

class GenesTableView : public QTableView
{
public:
    explicit GenesTableView(QWidget *parent = 0);
    virtual ~GenesTableView();

private:
    GeneFeatureItemModel *geneModel;
};

#endif // GENESTABLEVIEW_H
