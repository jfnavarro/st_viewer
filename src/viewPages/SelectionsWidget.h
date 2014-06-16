/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef SELECTIONSWIDGET_H
#define SELECTIONSWIDGET_H

#include <QWidget>

#include "dataModel/GeneSelection.h"

class QPushButton;
class QLineEdit;
class GeneSelectionTableView;
class GeneSelectionItemModel;
class QSortFilterProxyModel;

//This widgets is part of the CellView,
//it is componsed of the Genes Selection Table
//a search field and expor/save/print selection
//buttons
class SelectionsWidget : public QWidget
{
    Q_OBJECT

public:

    explicit SelectionsWidget(QWidget *parent = 0);
    virtual ~SelectionsWidget();

public slots:

    void slotLoadModel(const GeneSelection::selectedItemsList &geneList);
    void slotClearModel();

signals:

    void signalClearSelection();
    void signalSaveSelection();
    void signalExportSelection();

private:

    GeneSelectionItemModel *getModel();
    QSortFilterProxyModel *getProxyModel();

    QPushButton *m_saveSelection;
    QPushButton *m_exportSelection;
    QPushButton *m_clearSelection;
    QLineEdit *m_geneSelectionFilterLineEdit;
    GeneSelectionTableView *m_selections_tableview;

    Q_DISABLE_COPY(SelectionsWidget)
};

#endif // SELECTIONSWIDGET_H