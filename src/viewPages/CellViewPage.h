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
// which contains a table of genes and an OpenGL based
// rendering canvas to visualize the cell tissue and the genes.
// It also contains a toobar like every page. Functionalities in the toolbar are
// handled by slots.
// We do lazy inizialization of the visual stuff, specially OpenGL based stuff

// TODO add a cache for the visual settings
// TODO move visual settings to mainwindow as an independent widget
// TODO add a ruler visual object
// TODO add a visual object to show name of dataset
// TODO add a visual object to hightlight spots with color
// TODO add a lazo selection option
// TODO add an option to show normalized reads with DESeq
// TODO add an option to show the coordinates when the user hovers a spot
// TODO add an option to show gene expression in a defined color range
// TODO the thresholds and shared settings should be encapsulated in an object
// then the rendering objects just need to have a reference and update when anything changes
class CellViewPage : public QWidget
{
    Q_OBJECT

public:
    CellViewPage(QSharedPointer<DataProxy> dataProxy, QWidget *parent = 0);
    virtual ~CellViewPage();

    // clear the loaded content
    void clean();

signals:

    // notify the user has made a selection
    void signalUserSelection();
    // notify the user wants to log out
    void signalLogOut();

public slots:

    // the user has opened/edit/removed  a dataset
    void slotDatasetOpen(const QString &datasetId);
    void slotDatasetUpdated(const QString &datasetId);
    void slotDatasetRemoved(const QString &datasetId);
    // the user has cleared the selections
    void slotClearSelections();
    // the user has selected/deselected genes
    void slotGenesSelected(const DataProxy::GeneList &genes);
    // the user has changed the color of genes
    void slotGenesColor(const DataProxy::GeneList &genes);
    // the user has changed the cut off of a gene
    void slotGeneCutOff(const DataProxy::GenePtr gene);
    // set the user name for the tool bar field
    void slotSetUserName(const QString &username);

private slots:

    // some slots for gene actions that need adjustment of the value (scale)
    void slotGeneShape(int geneShape);
    void slotGeneIntensity(int geneIntensity);
    void slotGeneSize(int geneSize);

    // save current scene
    void slotSaveImage();
    void slotPrintImage();

    // selection of spot using a the reg-exp dialog that takes gene names as input
    void slotSelectByRegExp();

    // select gene visual mode
    void slotSetGeneVisualMode(QAction *action);

    // select legend anchor
    void slotSetLegendAnchor(QAction *action);

    // select minimap anchor
    void slotSetMiniMapAnchor(QAction *action);

    // to handle when the user want to store the current selection into a selection object
    void slotCreateSelection();

    // to load the cell tissue figure (tile it into textures)
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

    // OpenGL visualization objects
    QSharedPointer<MiniMapGL> m_minimap;
    QSharedPointer<HeatMapLegendGL> m_legend;
    QSharedPointer<GeneRendererGL> m_gene_plotter;
    QSharedPointer<ImageTextureGL> m_image;
    QSharedPointer<GridRendererGL> m_grid;

    // color dialog for the grid
    QScopedPointer<QColorDialog> m_colorDialogGrid;

    // User interface
    QScopedPointer<Ui::CellView> m_ui;

    // Features Reads Distribution
    QScopedPointer<AnalysisFRD> m_FDH;

    // Elements of the cell view visual settings menu
    QScopedPointer<QRadioButton> m_colorLinear;
    QScopedPointer<QRadioButton> m_colorLog;
    QScopedPointer<QRadioButton> m_colorExp;
    QScopedPointer<QRadioButton> m_poolingGenes;
    QScopedPointer<QRadioButton> m_poolingReads;
    QScopedPointer<QRadioButton> m_poolingTPMs;
    QScopedPointer<SpinBoxSlider> m_geneHitsThreshold;
    QScopedPointer<SpinBoxSlider> m_geneGenesThreshold;
    QScopedPointer<SpinBoxSlider> m_geneTotalReadsThreshold;
    QScopedPointer<QSlider> m_geneIntensitySlider;
    QScopedPointer<QSlider> m_geneSizeSlider;
    QScopedPointer<QComboBox> m_geneShapeComboBox;
    // reference to dataProxy
    QSharedPointer<DataProxy> m_dataProxy;
    // currently opened dataset
    mutable QString m_openedDatasetId;

    Q_DISABLE_COPY(CellViewPage)
};

#endif // CELLVIEWPAGE_H
