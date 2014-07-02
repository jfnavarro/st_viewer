/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef GENESTABLEVIEW_H
#define GENESTABLEVIEW_H

#include <QTableView>
#include <QItemSelection>

class GeneFeatureItemModel;
class SortGenesProxyModel;

class GenesTableView : public QTableView
{
    Q_OBJECT

public:

    explicit GenesTableView(QWidget *parent = 0);
    virtual ~GenesTableView();

    QItemSelection geneTableItemSelection() const;

public slots:

    void setGeneNameFilter(QString);

    void reset() override;

private:

    void createColorComboBoxes();

    GeneFeatureItemModel *m_geneModel;
    SortGenesProxyModel *m_sortGenesProxyModel;

    Q_DISABLE_COPY(GenesTableView)
};

#endif // GENESTABLEVIEW_H
