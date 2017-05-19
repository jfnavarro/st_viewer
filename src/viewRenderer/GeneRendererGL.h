#ifndef GENERENDERERGL_H
#define GENERENDERERGL_H

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

#include "viewPages/SettingsWidget.h"
#include "math/QuadTree.h"
#include "SelectionEvent.h"

#include "GraphicItemGL.h"

// Gene renderer is what renders the data on the CellGLView canvas.
// It uses data arrays (GeneData) to render trough shaders.
// It has some attributes and variables changeable by slots.
// To clarify, by index(spot) we mean the physical spot in the array
// and by feature we mean the gene-index combination
class GeneRendererGL : public GraphicItemGL
{
    Q_OBJECT

public:

    // Visualization data
    // TODO this approach to store and visualization data will be refactored soon
    // visualization data will be stored and encapsulated in an object and it will
    // have a much more memory/speed efficient way to access and compute the
    // visualization data.

    // list of unique spot indexes
    // Qt containers are faster than STL containers
    typedef QSet<int> IndexesList;
    // lookup quadtree type (spot indexes)
    typedef QuadTree<int, 8> GeneInfoQuadTree;

    GeneRendererGL(QObject *parent = 0);
    virtual ~GeneRendererGL();

    // data builder (create visualization data from the ST data)
    void generateData(const STData &data);

    // clears data containers and reset variables to default
    void clearData();

    // set the dimensions of the bounding rect, also for the QuadTree
    void setDimensions(const QRectF &border);

public slots:

    // slots to change visual atttributes
    void slotSetIntensity(float intensity);
    void slotSetSize(float size);

    // slots for the thresholds
    void setReadsThreshold(const int limit);
    void setTotalGenesThreshold(const int limit);
    void setTotalReadsThreshold(const int limit);

    // slots to set visual modes and color computations modes
    void setVisualMode(const SettingsWidget::SettingsWidget &mode);
    void setVisualTypeMode(const SettingsWidget::VisualTypeMode &mode);
    void setNormalizationMode(const SettingsWidget::NormalizationMode &mode);
    void slotSetGenesCutOff(bool enable);

signals:

protected:
    // override method that returns the drawing size of this element
    const QRectF boundingRect() const override;
    void draw(QOpenGLFunctionsVersion &qopengl_functions) override;

private:

    // renders the data
    void render();

    // reset quad tree to rect size
    void resetQuadTree(const QRectF &rect);

    // compiles and loads the shaders
    void setupShaders();

    // lookup data (features respesent counts, a feature = (gene,spot) count
    // index is the OpenGL index
    // just the set of indexes for convenience
    IndexesList m_indexes;
    // quad tree container (used to find by coordinates)
    GeneInfoQuadTree m_geneInfoQuadTree;

    // visual attributes
    float m_intensity;
    float m_size;

    // threshold limits for gene hits
    int m_thresholdReads;
    int m_thresholdTotalReads;
    int m_thresholdTotalGenes;

    // enable/disable genes cutoff
    bool m_genes_cutoff;

    // bounding rect area
    QRectF m_border;

    // visual mode
    GeneVisualMode m_visualMode;

    // pooling mode (by gene count or reads counts or tpm counts)
    Visual::GenePooledMode m_poolingMode;

    // color computing mode (exp - log - linear)
    Visual::GeneColorMode m_colorComputingMode;

    // to know if the rendering data is ready
    bool m_isInitialized;

    // reference to dataProxy
    QSharedPointer<DataProxy> m_dataProxy;

    // OpenGL rendering variables
    STData m_geneData;
    QOpenGLShaderProgram m_shader_program;

    Q_DISABLE_COPY(GeneRendererGL)
};
*/
#endif // GENERENDERERGL_H
