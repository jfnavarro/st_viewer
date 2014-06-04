/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef GENESELECTIONTABLEVIEW_H
#define GENESELECTIONTABLEVIEW_H

#include <QTableView>
#include "viewPages/GeneNamesTableView.h"

class GeneSelectionItemModel;

class GeneSelectionTableView : public GeneNamesTableView
{
    Q_OBJECT

public:

    explicit GeneSelectionTableView(QWidget *parent = 0);
    virtual ~GeneSelectionTableView();

    void setGeneSelectionItemModel(GeneSelectionItemModel *geneSelectionItemModel);

private:

    Q_DISABLE_COPY(GeneSelectionTableView)
};

#endif // GENESELECTIONTABLEVIEW_H
