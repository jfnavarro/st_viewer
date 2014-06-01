/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
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

    GeneSelectionItemModel *m_geneSelectionModel;

    Q_DISABLE_COPY(GeneSelectionTableView)
};

#endif // GENESELECTIONTABLEVIEW_H
