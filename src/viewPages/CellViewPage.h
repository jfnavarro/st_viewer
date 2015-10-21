/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef CELLVIEWPAGE_H
#define CELLVIEWPAGE_H

#include <QWidget>
#include "data/DataProxy.h"
#include <memory>

class QColorDialog;
class SelectionDialog;
class Error;
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

namespace Ui
{
class CellView;
} // namespace Ui

// This is the definition of the cell view page visualization widget
// which contains a table of genes, a table of selected genes and an OpenGL based
// rendering canvas to visualize the cell tissue and the genes.
// It also contains a toobar like every page. Functionalities in the toolbar are handled by slots.
// We do lazy inizialization of the visual stuff, specially openGL based stuff

// TODO add a cache for the visual settings
// TODO move visual settings to mainwindow
// TODO add a ruler visual object
// TODO add a visual object to show name of dataset
// TODO add a visual object to draw selections
// TODO add a lazo selection option
// TODO add hoover functionalities (specially if user hoovers a selection to show selection info)
// TODO add a global threshold in percentage for the visual settings
class CellViewPage : public QWidget
{
    Q_OBJECT

public:

    CellViewPage(QPointer<DataProxy> dataProxy, QWidget* parent = 0);
    virtual ~CellViewPage();

    // clear the loaded content
    void clean();

signals:

    // notify the user has made a selection
    void signalUserSelection();
    // notify the user wants to log out
    void signalLogOut();

public slots:

    // the user has opened a dataset
    // TODO check if the dataset is already opened
    void slotDatasetOpen(QString datasetId);
    // the user has cleared the selections
    void slotClearSelections();
    // the user wants to highlight a selection
    // TODO send IDs instead
    void slotShowSelection(const QVector<UserSelection>& selections);
    // the user has selected/deselected genes
    void slotGenesSelected(const DataProxy::GeneList& genes);
    // the user has changed the color of genes
    void slotGenesColor(const DataProxy::GeneList& genes);
    // set the user name for the tool bar field
    void slotSetUserName(const QString& username);

private slots:

    // some slots for gene actions that need adjustment of the value (scale)
    void slotGeneShape(int geneShape);
    void slotGeneIntensity(int geneIntensity);
    void slotGeneSize(int geneSize);
    void slotGeneBrightness(int geneBrightness);

    // save current scene
    void slotSaveImage();
    void slotPrintImage();

    // selection of genes using a the reg-exp dialog
    void slotSelectByRegExp();

    // select gene visual mode
    void slotSetGeneVisualMode(QAction* action);

    // select legend anchor
    void slotSetLegendAnchor(QAction* action);

    // select legend computation of values (reads or number of genes)
    void slotSetLegendType(QAction* action);

    // select minimap anchor
    void slotSetMiniMapAnchor(QAction* action);

    // to handle when the user makes a selection
    void slotSelectionUpdated();

    // to load the cell tissue figure (tile it into textures) asynchronously
    void slotLoadCellFigure();

private:
    // create OpenGL graphical elements and view
    void initGLView();

    // create menus, add them to the main bar and create the connections
    void createMenusAndConnections();

    // reset all the visual variables to default
    void resetActionStates();

    // to enable/disable main controls
    void setEnableButtons(bool enable);

    // internal function to initialize UI components
    void datasetContentDownloaded();

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

    // Elements of the cell view visual settings menu
    QPointer<QRadioButton> m_colorLinear;
    QPointer<QRadioButton> m_colorLog;
    QPointer<QRadioButton> m_colorExp;
    QPointer<QRadioButton> m_poolingGenes;
    QPointer<QRadioButton> m_poolingReads;
    QPointer<QRadioButton> m_poolingTPMs;
    QPointer<SpinBoxSlider> m_geneHitsThreshold;
    QPointer<SpinBoxSlider> m_geneGenesThreshold;
    QPointer<SpinBoxSlider> m_geneTotalReadsThreshold;
    // TODO temp not smart pointer to make the add addSliderToMenu work
    // solution is just use smart pointers but transfer ownership when invoking the function
    QSlider* m_geneIntensitySlider;
    QSlider* m_geneSizeSlider;
    QSlider* m_geneShineSlider;
    QSlider* m_geneBrightnessSlider;
    QPointer<QComboBox> m_geneShapeComboBox;
    // reference to dataProxy
    QPointer<DataProxy> m_dataProxy;

    Q_DISABLE_COPY(CellViewPage)
};

#endif // CELLVIEWPAGE_H
