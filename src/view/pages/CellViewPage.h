/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef CELLVIEWPAGE_H
#define CELLVIEWPAGE_H

#include "Page.h"
#include "controller/data/DataProxy.h"

class QMenu;
class QActionGroup;
class QAction;
class QWidgetAction;
class QColorDialog;
class QToolBar;
class SelectionDialog;
class GeneFeatureItemModel;
class Error;
class GraphicsViewGL;
class GraphicsSceneGL;
class GenePlotterGL;
class ImageItemGL;
class QMenuBar;
class HeatMapLegendGL;
class GeneSelectionItemModel;

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
    // callback when the image loading is done sync
    void slotLoadCellFigurePost();
    // save current scene
    void slotSaveImage();
    void slotPrintImage();
    void slotExportSelection();
    // activate selection of genes
    void slotActivateSelection(bool);
    void slotSelectByRegExp();
    // select gene visual mode
    void slotSetGeneVisualMode(QAction *action);
    // launch a color selector
    void slotLoadColor();
    
protected:

    //dont want to allow wheel events
    //virtual void wheelEvent(QWheelEvent* event) {event->ignore();}
    
    //init gui elements, signals and connections
    void initGLView();
    void initGLModel();
    void initGLConnections();
    void finalizeGL();
    void createActions();
    void createToolBar();
    void createConnections();
    void resetActionStates();

    //image loading function
    void loadCellFigureAsync(QIODevice *device);

private:
    
    // mvc model
    GeneFeatureItemModel *geneModel;
    GeneSelectionItemModel *geneSelectionModel;

    // have 2 scene items (cell tissue image and gene plotter)
    GraphicsSceneGL *scene;
    ImageItemGL *cell_tissue;
    GenePlotterGL *gene_plotter_gl;

    // ui view items
    HeatMapLegendGL *m_heatmap;

    // tool bar
    QToolBar *toolBar;

    // actions for toolbar
    QAction *actionNavigate_goBack;
    QAction *actionSave_save;
    QAction *actionSave_print;
    QAction *actionSelection_toggleSelectionMode;
    QAction *actionSelection_showSelectionDialog;

    QAction *actionZoom_zoomIn;
    QAction *actionZoom_zoomOut;

    QMenu *menu_genePlotter;
    QAction *actionShow_showGrid;
    QAction *actionShow_showGenes;
    QAction *actionColor_selectColorGenes;
    QAction *actionColor_selectColorGrid;

    QActionGroup *actionGroup_toggleVisualMode;
    QAction *actionShow_toggleNormal;
    QAction *actionShow_toggleDynamicRange;
    QAction *actionShow_toggleDynamicRangeGenes;
    QAction *actionShow_toggleHeatMap;

    QWidgetAction *actionWidget_geneHitsThreshold;
    QWidgetAction *actionWidget_geneIntensity;
    QWidgetAction *actionWidget_geneSize;
    QWidgetAction *actionWidget_geneShape;

    QMenu *menu_cellTissue;
    QActionGroup *actionGroup_cellTissue;
    QAction *actionShow_cellTissueBlue;
    QAction *actionShow_cellTissueRed;
    QAction *actionShow_showCellTissue;

    // color dialogs
    QColorDialog *colorDialog_genes;
    QColorDialog *colorDialog_grid;

    // selection dialogs
    SelectionDialog *selectionDialog;

    // is in selection mode?
    bool m_selection_mode;

    // User interface
    Ui::CellView *ui;
};

#endif // CELLVIEWPAGE_H
