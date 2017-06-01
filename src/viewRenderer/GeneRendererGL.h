#ifndef GENERENDERERGL_H
#define GENERENDERERGL_H

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

#include "data/STData.h"
#include "viewPages/SettingsWidget.h"

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

    GeneRendererGL(const SettingsWidget::Rendering &rendering_settings, QObject *parent = 0);
    virtual ~GeneRendererGL();

    // data builder (create visualization data from the ST data)
    void attachData(QSharedPointer<STData> data);

    // clears data containers and reset variables to default
    void clearData();

    // clears the selected spots
    void clearSelection();

public slots:

    // update the rendering data
    void slotUpdate();

signals:

protected:
    // override method that returns the drawing size of this element
    const QRectF boundingRect() const override;
    void draw(QOpenGLFunctionsVersion &qopengl_functions) override;
    void setSelectionArea(const SelectionEvent &event);

private:

    // compiles and loads the shaders
    void setupShaders();

    // bounding rect area
    QRectF m_border;

    // rendering settings
    const SettingsWidget::Rendering &m_rendering_settings;

    // rendering data
    QSharedPointer<STData> m_geneData;

    // OpenGL rendering shader
    QOpenGLShaderProgram m_shader_program;

    // true when the rendering data has been initialized
    bool m_initialized;

    Q_DISABLE_COPY(GeneRendererGL)
};

#endif // GENERENDERERGL_H
