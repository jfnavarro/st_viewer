/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef GENESELECTIONTABLEVIEW_H
#define GENESELECTIONTABLEVIEW_H

#include <QTableView>

class GeneSelectionItemModel;
class SortGenesProxyModel;

class GeneSelectionTableView : public QTableView
{
    Q_OBJECT

public:

    explicit GeneSelectionTableView(QWidget *parent = 0);
    virtual ~GeneSelectionTableView();

    QItemSelection geneTableItemSelection() const;

public slots:

    void setGeneNameFilter(QString);

private:

    GeneSelectionItemModel *m_geneSelectionModel;
    SortGenesProxyModel *m_sortGenesProxyModel;

    Q_DISABLE_COPY(GeneSelectionTableView)
};

#endif // GENESELECTIONTABLEVIEW_H
