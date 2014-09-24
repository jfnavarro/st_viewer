/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef CELLVIEWPAGE_H
#define CELLVIEWPAGE_H

#include <memory>
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
class AnalysisHistogram;

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
    
    explicit CellViewPage(QPointer<DataProxy> dataProxy, QWidget *parent = 0);
    virtual ~CellViewPage();

    // load the content of the currently selected dataset
    bool loadData();

public slots:
    
    void onEnter() override;
    void onExit() override;

private slots:
    
    // save current scene
    void slotSaveImage();
    void slotPrintImage();

    //shows a file dialog to save the current genes selection to a file
    void slotExportGenesSelection();
    
    //shows a file dialog to save the current features selection to a file
    //TODO this slot will be removed in release 0.5
    void slotExportFeaturesSelection();

    // selection of genes
    void slotSelectByRegExp();
    
    // select gene visual mode
    void slotSetGeneVisualMode(QAction *action);

    // select legend anchor
    void slotSetLegendAnchor(QAction *action);

    // select minimap anchor
    void slotSetMiniMapAnchor(QAction *action);

    // genes were selected
    void slotSelectionUpdated();

    // slot to save the currently selected genes
    void slotSaveSelection();

    // load the cell tissue figure into the stage
    void slotLoadCellFigure();

    // shows/hides the Features Distribution Histogram
    void slotSetFDHVisible(bool visible);

private:
    
    // create GL graphical elements and their connections
    void initGLView();
    void createGLConnections();
    
    // create tool bar
    void createToolBar();

    // create connections
    void createConnections();

    // reset all the visual variables to default
    void resetActionStates();

    // OpenGL visualization objects
    QPointer<MiniMapGL> m_minimap;
    QPointer<HeatMapLegendGL> m_legend;
    QPointer<GeneRendererGL> m_gene_plotter;
    QPointer<ImageTextureGL> m_image;
    QPointer<GridRendererGL> m_grid;
    QPointer<CellGLView> m_view;

    // color dialog for the grid
    QPointer<QColorDialog> m_colorDialogGrid;

    // tool bar
    QPointer<CellViewPageToolBar> m_toolBar;

    // User interface
    std::unique_ptr<Ui::CellView> m_ui;

    // reference to dataProxy
    QPointer<DataProxy> m_dataProxy;

    // Features Distribution Histogram
    QPointer<AnalysisHistogram> m_FDH;

    Q_DISABLE_COPY(CellViewPage)
};

#endif // CELLVIEWPAGE_H
