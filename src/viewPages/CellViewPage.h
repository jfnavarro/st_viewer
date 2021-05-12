#ifndef CELLVIEWPAGE_H
#define CELLVIEWPAGE_H

#include <QWidget>
#include <QFutureWatcher>

#include "data/Dataset.h"
#include "data/UserSelection.h"

class SelectionDialog;
class SettingsWidget;
class SpotsWidget;
class GenesWidget;
class ClustersWidget;
class UserSelectionsPage;
class AnalysisClustering;
class CellGLView3D;

namespace Ui
{
class CellView;
} // namespace Ui

// This is the Widget the represents the main visualization object of the tool.
// The image of the tissue and the data are visualized here.
// It contains references to the widget of genes, spots and clusters.
// The visualization (2D and 3D) is done using OpenGL and shaders.
// It contains a toolbar. Functionalities in the toolbar are handled by slots.
// We do lazy inizialization of the visual stuff, specially OpenGL based stuff
// It contains a widget with different visualization and data settings

// TODO add a ruler visual object
// TODO add a visual object to show the coordinates when the user hovers a spot
class CellViewPage : public QWidget
{
    Q_OBJECT

public:

    CellViewPage(QSharedPointer<SpotsWidget> spots,
                 QSharedPointer<GenesWidget> genes,
                 QSharedPointer<ClustersWidget> clusters,
                 QSharedPointer<UserSelectionsPage> user_selections,
                 QWidget *parent = nullptr);
    virtual ~CellViewPage() override;

    // clear the loaded dataset and reset settings
    void clear();

    // load the opened dataset
    void loadDataset(const Dataset &dataset);

    // the user has cleared the selections
    void clearSelections();

signals:

public slots:

private slots:

    // save/esport current view
    void slotSaveImage();
    void slotPrintImage();

    // user wants to show the QC widget
    void slotShowQC();

    // user wants to perform spot clustering
    void slotClustering();

    // user wants to load a file with clusters (spot - color)
    void slotLoadSpotClustersFile();

    // user wants to load a file with genes to select
    void slotLoadGenesColorsFile();

    // user has performed spot clustering in the clustering widget
    void slotLoadSpotClusters();

    // user has made a selection in the clustering widget
    void slotSelectSpotsClustering();

    // user wants to export the clusters as selections from the clustering widget
    void slotCreateClusteringSelections();

    // user wants to create a selection
    void slotCreateSelection();

private:

    // create all the connections
    void createConnections();

    // internal function to create cluster objects and update views
    void createClusters(const QMultiHash<int, QString> &clusters);

    // Reference to other views
    QSharedPointer<SpotsWidget> m_spots;
    QSharedPointer<GenesWidget> m_genes;
    QSharedPointer<ClustersWidget> m_clusters;
    QSharedPointer<UserSelectionsPage> m_user_selections;

    // GUI UI object
    QScopedPointer<Ui::CellView> m_ui;

    // different control widgets and views
    QScopedPointer<SettingsWidget> m_settings;

    // the spot clustering widget
    QScopedPointer<AnalysisClustering> m_clustering;

    // the currently opened dataset
    Dataset m_dataset;

    Q_DISABLE_COPY(CellViewPage)
};

#endif // CELLVIEWPAGE_H
