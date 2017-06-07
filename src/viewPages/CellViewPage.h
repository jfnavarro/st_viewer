#ifndef CELLVIEWPAGE_H
#define CELLVIEWPAGE_H

#include <QWidget>
#include "data/Dataset.h"
#include "data/UserSelection.h"
#include "viewRenderer/ImageTextureGL.h"
#include "viewRenderer/HeatMapLegendGL.h"
#include "viewRenderer/GeneRendererGL.h"

class SelectionDialog;
class CellGLView;
class SpotsWidget;
class GenesWidget;
class SettingsWidget;

namespace Ui
{
class CellView;
} // namespace Ui

// This is the definition of the cell view page visualization widget
// which contains a table of genes/spots and an OpenGL based
// rendering canvas to visualize the cell tissue and the spots (dataset).
// It also contains a toobar. Functionalities in the toolbar are handled by slots.
// We do lazy inizialization of the visual stuff, specially OpenGL based stuff
// It contains a widget with different visualization and data settings

// TODO add a ruler visual object
// TODO add a lazo selection option
// TODO add tissue detection selection (Ludvig's)
// TODO add a visual object to show the coordinates when the user hovers a spot
class CellViewPage : public QWidget
{
    Q_OBJECT

public:
    CellViewPage(QWidget *parent = 0);
    virtual ~CellViewPage();

    // clear the loaded dataset and reset settings
    void clear();

    // to handle when the user want to store the current selection into a selection object
    // the function will create an UserSelection object and send it to the SelectionsPage
    UserSelection createSelection();

    // true if the user has made a valid selection and it is active
    bool hasSelection();

    // the user has opened/edit a dataset
    void loadDataset(const Dataset &dataset);

    // the user has cleared the selections
    void clearSelections();

signals:

    // notify the user has made a selection
    void signalUserSelection();

public slots:

    // the user has updated the genes
    void slotGenesUpdate();

    // the user has updated the spots
    void slotSpotsUpdated();

private slots:

    // save/esport current view
    void slotSaveImage();
    void slotPrintImage();

    // selection of spots using a the reg-exp dialog that takes gene names as input
    void slotSelectByRegExp();

private:
    // create OpenGL graphical elements and view
    void initRenderer();

    // create all the connections
    void createConnections();

    // GUI UI object
    QScopedPointer<Ui::CellView> m_ui;

    // OpenGL visualization objects
    QSharedPointer<HeatMapLegendGL> m_legend;
    QSharedPointer<GeneRendererGL> m_gene_plotter;
    QSharedPointer<ImageTextureGL> m_image;

    // different control widgets and views
    QScopedPointer<SettingsWidget> m_settings;

    // the currently opened dataset
    Dataset m_dataset;

    Q_DISABLE_COPY(CellViewPage)
};

#endif // CELLVIEWPAGE_H
