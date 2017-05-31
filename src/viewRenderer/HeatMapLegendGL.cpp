#include "HeatMapLegendGL.h"

#include <QPainter>
#include <QImage>
#include <QApplication>
#include <QVector2D>
#include <QLabel>

#include "math/Common.h"
#include "color/HeatMap.h"

static const float legend_x = 0.0;
static const float legend_y = 0.0;
static const float legend_width = 30.0;
static const float legend_height = 200.0;
static const float bars_width = 40.0;

HeatMapLegendGL::HeatMapLegendGL(const SettingsWidget::Rendering &rendering_settings, QObject *parent)
    : GraphicItemGL(parent)
    , m_texture(QOpenGLTexture::Target2D)
    , m_textureText(QOpenGLTexture::Target2D)
    , m_texture_vertices()
    , m_texture_cords()
    , m_rendering_settings(rendering_settings)
    , m_initialized(false)
{
    setVisualOption(GraphicItemGL::Transformable, false);
    setVisualOption(GraphicItemGL::Visible, false);
    setVisualOption(GraphicItemGL::Selectable, false);
    setVisualOption(GraphicItemGL::Yinverted, false);
    setVisualOption(GraphicItemGL::Xinverted, false);
    setVisualOption(GraphicItemGL::RubberBandable, false);
    setAnchor(Anchor::NorthEast);
}

HeatMapLegendGL::~HeatMapLegendGL()
{
    clearData();
}

void HeatMapLegendGL::clearData()
{
    if (m_texture.isCreated()) {
        m_texture.destroy();
    }

    m_texture_vertices.clear();
    m_texture_cords.clear();
    m_texture_cords.clear();

    if (m_textureText.isCreated()) {
        m_textureText.destroy();
    }

    m_initialized = false;
}

void HeatMapLegendGL::slotUpdate()
{
    generateHeatMap();
}

void HeatMapLegendGL::draw(QOpenGLFunctionsVersion &qopengl_functions)
{
    if (!m_initialized) {
        return;
    }

    qopengl_functions.glEnable(GL_TEXTURE_2D);
    {
        // draw heatmap texture
        m_texture.bind();
        qopengl_functions.glBegin(GL_QUADS);
        {
            for (int i = 0; i < m_texture_vertices.size(); ++i) {
                qopengl_functions.glTexCoord2f(m_texture_cords.at(i).x(),
                                               m_texture_cords.at(i).y());
                qopengl_functions.glVertex2f(m_texture_vertices.at(i).x(),
                                             m_texture_vertices.at(i).y());
            }
        }
        qopengl_functions.glEnd();
        m_texture.release();

        // draw borders
        qopengl_functions.glBegin(GL_LINE_LOOP);
        {
            qopengl_functions.glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            for (QVector2D indice : m_texture_vertices) {
                qopengl_functions.glVertex2f(indice.x(), indice.y());
            }
        }
        qopengl_functions.glEnd();
        // get the min max values
        const float min = m_rendering_settings.legend_min;
        const float max = m_rendering_settings.legend_max;
        // draw text (add 5 pixels offset to the right)
        drawText(QPointF(legend_x + legend_width + 5, 0), QString::number(max),
                 qopengl_functions);
        drawText(QPointF(legend_x + legend_width + 5, legend_height), QString::number(min),
                 qopengl_functions);
    }
    qopengl_functions.glDisable(GL_TEXTURE_2D);
}

void HeatMapLegendGL::generateHeatMap()
{
    // get the min max values
    const float min = m_rendering_settings.legend_min;
    const float max = m_rendering_settings.legend_max;
    // generate image texture with the size of the legend and then fill it up with the colors
    // using the min-max values of the threshold and the color mode
    QImage image(legend_width, legend_height, QImage::Format_ARGB32);
    Color::ColorGradients cmap =
            m_rendering_settings.visual_mode == SettingsWidget::VisualMode::ColorRange?
                Color::ColorGradients::gpHot : Color::ColorGradients::gpSpectrum;
    Color::createLegend(image, min, max, cmap);
    // update the OpenGL texture
    m_texture.destroy();
    m_texture.create();
    m_texture.setMinificationFilter(QOpenGLTexture::Linear);
    m_texture.setMagnificationFilter(QOpenGLTexture::Linear);
    m_texture.setWrapMode(QOpenGLTexture::ClampToEdge);
    m_texture.setData(image);
    m_texture_vertices.append(QVector2D(legend_x, legend_y));
    m_texture_vertices.append(QVector2D(legend_x + legend_width, legend_y));
    m_texture_vertices.append(QVector2D(legend_x + legend_width, legend_y + legend_height));
    m_texture_vertices.append(QVector2D(legend_x, legend_y + legend_height));
    m_texture_cords.append(QVector2D(0.0, 0.0));
    m_texture_cords.append(QVector2D(1.0, 0.0));
    m_texture_cords.append(QVector2D(1.0, 1.0));
    m_texture_cords.append(QVector2D(0.0, 1.0));
    m_initialized = true;
}

void HeatMapLegendGL::drawText(const QPointF &posn, const QString &str,
                               QOpenGLFunctionsVersion &qopengl_functions)
{
    // Create an image from the text
    QFont monoFont("Courier", 12, QFont::Normal);
    QFontMetrics metrics(monoFont);
    QRect textRect = metrics.boundingRect(str);
    QImage image(textRect.size(), QImage::Format_ARGB32);
    image.fill(0);
    QPainter qpainter(&image);
    qpainter.setFont(monoFont);
    qpainter.setPen(Qt::black);
    qpainter.setRenderHint(QPainter::Antialiasing, true);
    qpainter.drawText(textRect.x(), metrics.ascent(), str);
    qpainter.end();
    // Update the OpenGL texture with the text image
    m_textureText.destroy();
    m_textureText.create();
    m_textureText.setData(image.mirrored());
    const int x = posn.x();
    const int y = posn.y();
    QVector<QVector2D> vertices;
    vertices.append(QVector2D(x + textRect.x(), y + metrics.ascent()));
    vertices.append(QVector2D(x + textRect.x(), y - metrics.descent()));
    vertices.append(QVector2D(x + textRect.x() + textRect.width(), y - metrics.descent()));
    vertices.append(QVector2D(x + textRect.x() + textRect.width(), y + metrics.ascent()));
    QVector<QVector2D> texCoord;
    texCoord.append(QVector2D(0.0, 0.0));
    texCoord.append(QVector2D(0.0, 1.0));
    texCoord.append(QVector2D(1.0, 1.0));
    texCoord.append(QVector2D(1.0, 0.0));
    // Draw the texture
    m_textureText.bind();
    qopengl_functions.glBegin(GL_QUADS);
    {
        for (int i = 0; i < vertices.size(); ++i) {
            qopengl_functions.glTexCoord2f(texCoord.at(i).x(), texCoord.at(i).y());
            qopengl_functions.glVertex2f(vertices.at(i).x(), vertices.at(i).y());
        }
    }
    qopengl_functions.glEnd();
    m_textureText.release();
}

const QRectF HeatMapLegendGL::boundingRect() const
{
    return QRectF(legend_x, legend_y, legend_width + bars_width, legend_height);
}
