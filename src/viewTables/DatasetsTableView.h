/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef DATASETSTABLEVIEW_H
#define DATASETSTABLEVIEW_H

#include <QTableView>
#include <QPointer>

class DatasetItemModel;
class QSortFilterProxyModel;

class DatasetsTableView : public QTableView
{
    Q_OBJECT

public:

    explicit DatasetsTableView(QWidget *parent = 0);
    virtual ~DatasetsTableView();

    // returns the current selection mapped to the sorting model
    QItemSelection datasetsTableItemSelection() const;

private:
    //references to model and proxy model
    QPointer<DatasetItemModel> m_datasetModel;
    QPointer<QSortFilterProxyModel> m_sortDatasetsProxyModel;

    Q_DISABLE_COPY(DatasetsTableView)
};

#endif // DATASETSTABLEVIEW_H //
