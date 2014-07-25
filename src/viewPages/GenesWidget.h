/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GENESWIDGET_H
#define GENESWIDGET_H

#include <QWidget>

#include "data/DataProxy.h"

class QPushButton;
class QLineEdit;
class GenesTableView;
class GeneFeatureItemModel;
class QSortFilterProxyModel;
class ColorListEditor;

//This widget is part of the CellView,
//it is componsed of the Gene Table
//a search field and the select and action
//menus

//TODO current approach with menus gives problems
//in MAC, replaced temporary for push buttons
class GenesWidget : public QWidget
{
    Q_OBJECT

public:

    explicit GenesWidget(QWidget *parent = 0);
    virtual ~GenesWidget();

    //clear focus/status and selections
    void clear();

signals:

    void signalSelectionChanged(DataProxy::GeneList);
    void signalColorChanged(DataProxy::GeneList);

public slots:

    void slotSetColorAllSelected(const QColor &color);
    void slotSetVisibilityForSelectedRows(bool visible);

    //updates the model of the table
    void slotLoadModel(DataProxy::GeneList &geneList);

private slots:

    void slotHideAllSelected();
    void slotShowAllSelected();

 private:

    QSortFilterProxyModel *getProxyModel();
    GeneFeatureItemModel *getModel();

    //QPointer<QPushButton> m_selectionMenu;
    QPointer<QPushButton> m_selectionAllButton;
    QPointer<QPushButton> m_selectionClearAllButton;
    //QPointer<QPushButton> m_actionMenu;
    QPointer<QLineEdit> m_lineEdit;
    QPointer<GenesTableView> m_genes_tableview;
    QPointer<ColorListEditor> m_colorList;
    QPointer<QPushButton> m_showSelectedButton;
    QPointer<QPushButton> m_hideSelectedButton;

    Q_DISABLE_COPY(GenesWidget)
};

#endif // GENESWIDGET_H
