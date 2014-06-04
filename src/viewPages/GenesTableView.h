/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef GENESTABLEVIEW_H
#define GENESTABLEVIEW_H

#include <QTableView>
#include <QItemSelection>
#include "viewPages/GeneNamesTableView.h"

class GeneFeatureItemModel;

class GenesTableView : public GeneNamesTableView
{
public:
    explicit GenesTableView(QWidget *parent = 0);
    virtual ~GenesTableView();

public:

    void setGeneFeatureItemModel(GeneFeatureItemModel *geneModel);

private:

    Q_DISABLE_COPY(GenesTableView)
};

#endif // GENESTABLEVIEW_H
