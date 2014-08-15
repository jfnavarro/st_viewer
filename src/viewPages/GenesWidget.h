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
class QColorDialog;

//This widget is part of the CellView,
//it is componsed of the Gene Table
//a search field and the select and action
//menus

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
    void slotLoadModel(const DataProxy::GeneList &geneList);

private slots:

    void slotHideAllSelected();
    void slotShowAllSelected();

 private:

    QSortFilterProxyModel *getProxyModel();
    GeneFeatureItemModel *getModel();

    QPointer<QPushButton> m_selectionAllButton;
    QPointer<QPushButton> m_selectionClearAllButton;
    QPointer<QLineEdit> m_lineEdit;
    QPointer<GenesTableView> m_genes_tableview;
    QPointer<QColorDialog> m_colorList;
    QPointer<QPushButton> m_showColorButton;
    QPointer<QPushButton> m_showSelectedButton;
    QPointer<QPushButton> m_hideSelectedButton;

    Q_DISABLE_COPY(GenesWidget)
};

#endif // GENESWIDGET_H
