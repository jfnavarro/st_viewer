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

namespace Ui{
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
    void loadData();

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

    // graphical items
    MiniMapGL *m_minimap = nullptr;
    HeatMapLegendGL *m_legend = nullptr;
    GeneRendererGL *m_gene_plotter = nullptr;
    ImageTextureGL *m_image = nullptr;
    GridRendererGL *m_grid = nullptr;
    CellGLView *m_view = nullptr;

    // selection dialogs
    SelectionDialog *selectionDialog = nullptr;

    // color dialogs
    QColorDialog *m_colorDialogGenes = nullptr;
    QColorDialog *m_colorDialogGrid = nullptr;

    // tool bar
    CellViewPageToolBar *m_toolBar;

    // User interface
    Ui::CellView *ui;

    Q_DISABLE_COPY(CellViewPage)
};

#endif // CELLVIEWPAGE_H
