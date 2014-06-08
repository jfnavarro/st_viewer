/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef CELLVIEWPAGE_H
#define CELLVIEWPAGE_H

#include "Page.h"

#include "data/DataProxy.h"

class QColorDialog;
class SelectionDialog;
class Error;
class CellViewPageToolBar;
class CellGLView;
class ImageTextureGL;
class GridRendererGL;
class HeatMapLegendGL;
class MiniMapGL;
class GeneRendererGL;

namespace Ui
{
class CellView;
} // namespace Ui

// this is the definition of the cell view page which contains a list of genes and an OpenGL based
// rendering canvas to visualize the cell tissue and the genes.
// as every page it implements the moveToNextPage and moveToPreviousPage
// the methods onEnter and onExit are called dynamically from the page manager.
// we do lazy inizialization of the visual stuff, specially openGL based stuff

class CellViewPage : public Page
{
    Q_OBJECT

public:
    
    explicit CellViewPage(QWidget *parent = 0);
    virtual ~CellViewPage();

    // load the content of the currently selected dataset
    bool loadData();

public slots:
    
    void onInit();
    void onEnter();
    void onExit();

protected slots:
    
    // load the cell tissue figure into the stage
    void slotLoadCellFigure();
    
    // save current scene
    void slotSaveImage();
    void slotPrintImage();

    //shows a file dialog to save the current gene selection to a file
    void slotExportSelection();
    
    // selection of genes
    void slotSelectByRegExp();
    
    // select gene visual mode
    void slotSetGeneVisualMode(QAction *action);

    // select legend anchor
    void slotSetLegendAnchor(QAction *action);

    // select minimap anchor
    void slotSetMiniMapAnchor(QAction *action);

    // launch a color selector
    void slotLoadColor();

    // genes were selected
    void slotSelectionUpdated();

    // slot to save the currently selected genes
    void slotSaveSelection();

protected:
    
    // create GL graphical elements and their connections
    void initGLView();
    void createGLConnections();
    
    // create tool bar
    void createToolBar();

    // create connections
    void createConnections();

    // reset all the visual variables to default
    void resetActionStates();

private:

    QSharedPointer<MiniMapGL> m_minimap;
    QSharedPointer<HeatMapLegendGL> m_legend;
    QSharedPointer<GeneRendererGL> m_gene_plotter;
    QSharedPointer<ImageTextureGL> m_image;
    QSharedPointer<GridRendererGL> m_grid;
    QSharedPointer<CellGLView> m_view;

    // color dialogs
    QScopedPointer<QColorDialog> m_colorDialogGenes;
    QScopedPointer<QColorDialog> m_colorDialogGrid;

    // tool bar
    QScopedPointer<CellViewPageToolBar> m_toolBar;

    // User interface
    Ui::CellView *ui;

    Q_DISABLE_COPY(CellViewPage)
};

#endif // CELLVIEWPAGE_H
