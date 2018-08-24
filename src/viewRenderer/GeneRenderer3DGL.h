#ifndef GENERENDERER3DGL_H
#define GENERENDERER3DGL_H

#include "data/STData.h"
#include "viewPages/SettingsWidget.h"

#include "GraphicItemGL.h"

// GeneRenderer3DGL is what renders the data (3D) on the CellGLView canvas.
// It uses QDataVisualization to paint dots (spots) with colours
class GeneRendererGL : public GraphicItemGL
{
    Q_OBJECT

public:

    GeneRendererGL(SettingsWidget::Rendering &rendering_settings, QObject *parent = nullptr);
    virtual ~GeneRendererGL() override;

    // data builder (create visualization data from the ST data)
    void attachData(QSharedPointer<STData> data);

    // clears data containers and reset variables to default
    void clearData();

public slots:

    // update the rendering data
    void slotUpdate();

signals:

protected:
    // override method that returns the drawing size of this element
    const QRectF boundingRect() const override;
    void draw(QOpenGLFunctionsVersion &qopengl_functions, QPainter &painter) override;
    void setSelectionArea(const SelectionEvent &event) override;

private:

    // compiles and loads the shaders
    void setupShaders();

    // bounding rect area
    QRectF m_border;

    // rendering settings
    SettingsWidget::Rendering &m_rendering_settings;

    // rendering data
    QSharedPointer<STData> m_geneData;

    // true when the rendering data has been initialized
    bool m_initialized;

    Q_DISABLE_COPY(GeneRendererGL)
};

#endif // GENERENDERER3DGL_H
