/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef CELLVIEWPAGE_H
#define CELLVIEWPAGE_H

#include "Page.h"
#include "core/data/DataProxy.h"

class QColorDialog;
class SelectionDialog;
class Error;
class GenePlotterGL;
class CellViewPageToolBar;
class CellGLView;
class ImageTextureGL;
class GridRendererGL;

namespace Ui
{
class CellView;
} // namespace Ui //

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

public slots:
    
    void onInit();
    void onEnter();
    void onExit();

protected slots:
    
    // load the cell tissue figure into the stage (can be done sync and async)
    void slotLoadCellFigure();
    
    // save current scene
    void slotSaveImage();
    void slotPrintImage();
    void slotExportSelection();
    
    // activate selection of genes
    void slotActivateSelection(bool);
    void slotSelectByRegExp();
    
    // select gene visual mode
    void slotSetGeneVisualMode(QAction *action);
    // select threshold mode
    void slotSetGeneThresholdMode(QAction *action);
    
    // launch a color selector
    void slotLoadColor();

protected:
    
    //init gui elements, signals and connections
    void initGLView();
    void initGLModel();
    void initGLConnections();
    void finishGLConnections();
    void finalizeGL();
    
    // create tool bar
    void createToolBar();
    // create connections
    void createConnections();
    // reset all the visual variables to default
    void resetActionStates();

private:

    // graphical items
    //MiniMapGL *m_minimap;
    //LegendGL *m_legend;
    GenePlotterGL *gene_plotter_gl = nullptr;
    ImageTextureGL *m_image = nullptr;
    GridRendererGL *m_grid = nullptr;
    CellGLView *view = nullptr;

    // selection dialogs
    SelectionDialog *selectionDialog = nullptr;

    // color dialogs
    QColorDialog *colorDialog_genes = nullptr;
    QColorDialog *colorDialog_grid = nullptr;

    // tool bar
    CellViewPageToolBar *toolBar;

    // is in selection mode?
    bool m_selection_mode;

    // User interface
    Ui::CellView *ui;
};

#endif // CELLVIEWPAGE_H
