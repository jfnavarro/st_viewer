#include "GeneRendererGL.h"

#include <QApplication>
#include <QPainter>

#include "color/HeatMap.h"

// hash function for QColor for use in QSet / QHash
QT_BEGIN_NAMESPACE
uint qHash(const QColor &c)
{
    return qHash(c.rgba());
}
QT_END_NAMESPACE

GeneRendererGL::GeneRendererGL(SettingsWidget::Rendering &rendering_settings, QObject *parent)
    : GraphicItemGL(parent)
    , m_rendering_settings(rendering_settings)
    , m_initialized(false)
{
    setVisualOption(GraphicItemGL::Transformable, true);
    setVisualOption(GraphicItemGL::Visible, true);
    setVisualOption(GraphicItemGL::Selectable, false);
    setVisualOption(GraphicItemGL::Yinverted, false);
    setVisualOption(GraphicItemGL::Xinverted, false);
    setVisualOption(GraphicItemGL::RubberBandable, true);
    setAnchor(GraphicItemGL::Anchor::None);

    // initialize variables
    clearData();
}

GeneRendererGL::~GeneRendererGL()
{
}

void GeneRendererGL::clearData()
{
    m_initialized = false;
}

void GeneRendererGL::slotUpdate()
{
    if (m_initialized) {
        m_geneData->computeRenderingData(m_rendering_settings);
    }
}

void GeneRendererGL::attachData(QSharedPointer<STData> data)
{
    m_geneData = data;
    m_initialized = true;
    m_border = m_geneData->getBorder();
}

void GeneRendererGL::draw(QOpenGLFunctionsVersion &qopengl_functions, QPainter &painter)
{
    Q_UNUSED(qopengl_functions);
    Q_UNUSED(painter);

    if (!m_initialized) {
        return;
    }
}

const QRectF GeneRendererGL::boundingRect() const
{
    return m_border;
}

void GeneRendererGL::setSelectionArea(const SelectionEvent &event)
{
    m_geneData->selectSpots(event);
    slotUpdate();
    emit updated();
}
