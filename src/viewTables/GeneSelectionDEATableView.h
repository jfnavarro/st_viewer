/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GENESELECTIONDEATABLEVIEW_H
#define GENESELECTIONDEATABLEVIEW_H

#include <QTableView>
#include <QPointer>

class SortGenesProxyModel;
class GeneSelectionDEAItemModel;

class GeneSelectionDEATableView : public QTableView
{
    Q_OBJECT

public:

    explicit GeneSelectionDEATableView(QWidget *parent = 0);
    virtual ~GeneSelectionDEATableView();

    // returns the current selection mapped to the sorting model
    QItemSelection geneTableItemSelection() const;

public slots:
    //slot used to set a search filter for the table
    void setGeneNameFilter(QString);

private:
    //references to model and proxy model
    QPointer<GeneSelectionDEAItemModel> m_geneSelectionDEAModel;
    QPointer<SortGenesProxyModel> m_sortGenesProxyModel;

    Q_DISABLE_COPY(GeneSelectionDEATableView)
};

#endif // GENESELECTIONDEATABLEVIEW_H
