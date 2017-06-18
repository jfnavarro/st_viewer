#include "GeneRendererGL.h"

#include <QFutureWatcher>
#include <QtConcurrent>
#include <QOpenGLShaderProgram>
#include <QImageReader>
#include <QApplication>
#include <QPainter>

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
    Q_UNUSED(qopengl_functions)

    if (!m_initialized) {
        return;
    }

    const auto spots = m_geneData->renderingSpots();
    const auto colors = m_geneData->renderingColors();
    const auto selecteds = m_geneData->renderingSelected();
    const double size = m_rendering_settings.size / 2;
    const double size_selected = size / 4;
    QPen pen;
    painter.setBrush(Qt::NoBrush);
    for (int i = 0; i < spots.size(); ++i) {
        const auto spot  = spots.at(i);
        const double x = spot.first;
        const double y = spot.second;
        const QColor color = colors.at(i);
        const bool selected = selecteds.at(i);
        // draw spot
        pen.setColor(color);
        pen.setWidthF(size);
        painter.setPen(pen);
        painter.drawEllipse(QRectF(x, y, size, size));
        if (selected) {
            pen.setColor(Qt::white);
            pen.setWidthF(size_selected);
            painter.setPen(pen);
            painter.drawEllipse(QRectF(x, y, size, size));
        }
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
