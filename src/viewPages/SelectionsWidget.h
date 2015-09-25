/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef SELECTIONSWIDGET_H
#define SELECTIONSWIDGET_H

#include <QDockWidget>
#include <QPointer>

#include "dataModel/UserSelection.h"

class QLineEdit;
class GeneSelectionTableView;
class GeneSelectionItemModel;
class QSortFilterProxyModel;

// This widgets is part of the CellView,
// it is composed of the aggregated genes from an user selection in a table
// which shows the genes and their values and a search box
class SelectionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SelectionsWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~SelectionsWidget();

    // clear focus/status and selections
    void clear();

public slots:

    // reload the model with the objects given as input
    void slotLoadModel(const UserSelection::selectedGenesList& geneList);

signals:

private:
    // internal functions to obtain the model and the proxy model of the table
    GeneSelectionItemModel* getModel();
    QSortFilterProxyModel* getProxyModel();

    // some references needed to UI elements
    QPointer<QLineEdit> m_geneSelectionFilterLineEdit;
    QPointer<GeneSelectionTableView> m_selections_tableview;

    Q_DISABLE_COPY(SelectionsWidget)
};

#endif // SELECTIONSWIDGET_H
