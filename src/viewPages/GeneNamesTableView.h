/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef GENENAMESSTABLEVIEW_H
#define GENENAMESSTABLEVIEW_H

#include <QTableView>
#include <QItemSelection>
#include <QString>

class GeneFeatureItemModel;
class SortGenesProxyModel;

class GeneNamesTableView : public QTableView
{
    Q_OBJECT

public:

    explicit GeneNamesTableView(QWidget *parent = 0);
    virtual ~GeneNamesTableView();

    QItemSelection geneTableItemSelection() const;

public slots:
  
    void setGeneNameFilter(QString);

protected:

    SortGenesProxyModel *m_sortGenesProxyModel;

private:

    Q_DISABLE_COPY(GeneNamesTableView)
};

#endif // GENENAMESSTABLEVIEW_H
