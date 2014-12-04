/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef CELLVIEWPAGE_H
#define CELLVIEWPAGE_H

#include "Page.h"

#include "data/DataProxy.h"
#include <memory>

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
class AnalysisFRD;
class QSlider;
class SpinBoxSlider;
class QComboBox;
class QRadioButton;

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
    
    CellViewPage(QPointer<DataProxy> dataProxy, QWidget *parent = 0);
    virtual ~CellViewPage();

public slots:
    
    void onEnter() override;
    void onExit() override;

private slots:
    
    //some slots for gene actions that need adjustment of the value
    void slotGeneShape(int geneShape);
    void slotGeneIntensity(int geneIntensity);
    void slotGeneSize(int geneSize);
    void slotGeneBrightness(int geneBrightness);

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

    // load the cell tissue figure (tile it into textures) async
    void slotLoadCellFigure();

    //used to be notified when the dataset content has been downloaded or a selection created
    //status contains the status of the operation (ok, abort, error)
    void slotImageAlignmentDownloaded(const DataProxy::DownloadStatus status);
    void slotDatasetContentDownloaded(const DataProxy::DownloadStatus status);

private:
    
    // create OpenGL graphical elements and view
    void initGLView();

    // create menus, add them to the main bar and create the connections
    void createMenusAndConnections();

    // reset all the visual variables to default
    void resetActionStates();

    // to enable/disable main controls
    void setEnableButtons(bool enable);

    // OpenGL visualization objects
    QPointer<MiniMapGL> m_minimap;
    QPointer<HeatMapLegendGL> m_legend;
    QPointer<GeneRendererGL> m_gene_plotter;
    QPointer<ImageTextureGL> m_image;
    QPointer<GridRendererGL> m_grid;
    QPointer<CellGLView> m_view;

    // color dialog for the grid
    QPointer<QColorDialog> m_colorDialogGrid;

    // User interface
    std::unique_ptr<Ui::CellView> m_ui;

    // Features Reads Distribution
    QPointer<AnalysisFRD> m_FDH;

    // elements of the gene/cell view settings menu
    QPointer<QRadioButton> m_colorLinear;
    QPointer<QRadioButton> m_colorLog;
    QPointer<QRadioButton> m_colorExp;
    QPointer<QRadioButton> m_poolingGenes;
    QPointer<QRadioButton> m_poolingReads;
    QPointer<QRadioButton> m_poolingTPMs;
    QPointer<SpinBoxSlider> m_geneHitsThreshold;
    //TODO temp not smart pointer to make the add addSliderToMenu work
    //solution is just use smart pointers but transfer ownership when invoking the function
    QSlider *m_geneIntensitySlider;
    QSlider *m_geneSizeSlider;
    QSlider *m_geneShineSlider;
    QSlider *m_geneBrightnessSlider;
    QPointer<QComboBox> m_geneShapeComboBox;

    Q_DISABLE_COPY(CellViewPage)
};

#endif // CELLVIEWPAGE_H
