/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef SELECTIONSWIDGET_H
#define SELECTIONSWIDGET_H

#include <QWidget>
#include <QPointer>
#include <QIcon>

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

    //clear focus/status and selections
    void clear();

public slots:

    void slotLoadModel(const GeneSelection::selectedItemsList &geneList);

signals:

    void signalClearSelection();
    void signalSaveSelection();
    void signalExportGenesSelection();
    void signalExportFeaturesSelection();

private:

    //internal function to configure created buttons
    //to avoid code duplication
    //TODO better approach would be to have factories somewhere else
    void configureButton(QPushButton *button,
                         const QIcon icon = QIcon(), const QString tooltip = QString());

    GeneSelectionItemModel *getModel();
    QSortFilterProxyModel *getProxyModel();

    //some references needed to UI elements
    QPointer<QLineEdit> m_geneSelectionFilterLineEdit;
    QPointer<GeneSelectionTableView> m_selections_tableview;

    Q_DISABLE_COPY(SelectionsWidget)
};

#endif // SELECTIONSWIDGET_H
