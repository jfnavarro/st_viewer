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

    if (!m_initialized) {
        return;
    }

    const bool is_dynamic =
            m_rendering_settings.visual_mode == SettingsWidget::VisualMode::DynamicRange;
    const bool do_values = m_rendering_settings.visual_mode != SettingsWidget::VisualMode::Normal;

    const auto &spots = m_geneData->spots();
    const auto &visibles = m_geneData->renderingVisible();
    const auto &colors = m_geneData->renderingColors();
    const auto &selecteds = m_geneData->renderingSelected();
    const auto &values = m_geneData->renderingValues();
    const double size = m_rendering_settings.size / 2;
    const double size_selected = size / 4;
    const double min_value = m_rendering_settings.legend_min;
    const double max_value = m_rendering_settings.legend_max;
    const double intensity = m_rendering_settings.intensity;

    QPen pen;
    painter.setBrush(Qt::NoBrush);
    for (int i = 0; i < spots.size(); ++i) {
        const bool visible = visibles.at(i);
        const auto spot  = spots.at(i)->adj_coordinates();
        const double x = spot.x;
        const double y = spot.y;
        if (visible) {
            const bool selected = selecteds.at(i);
            const double value = values.at(i);
            QColor color = colors.at(i);
            if (do_values && !spots.at(i)->visible()) {
                color = Color::adjustVisualMode(color, value, min_value,
                                                max_value, m_rendering_settings.visual_mode);
            }
            if (!is_dynamic) {
                color.setAlphaF(intensity);
            }
            pen.setColor(selected ? Qt::white : color);
            pen.setWidthF(selected ? size_selected : size);
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
