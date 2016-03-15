#include "GeneData.h"

#include "utils/Utils.h"

static const int QUAD_SIZE = 4;
static const QVector2D ta(0.0, 0.0);
static const QVector2D tb(0.0, 1.0);
static const QVector2D tc(1.0, 1.0);
static const QVector2D td(1.0, 0.0);

namespace
{

QColor fromOpenGLColor(const QVector4D opengl_color)
{
    return QColor::fromRgbF(opengl_color.x(), opengl_color.y(), opengl_color.z(), opengl_color.w());
}

QVector4D fromQtColor(const QColor color)
{
    return QVector4D(color.redF(), color.greenF(), color.blueF(), color.alphaF());
}
}
GeneData::GeneData()
{
}

GeneData::~GeneData()
{
}

void GeneData::clearData()
{
    m_vertices.clear();
    m_textures.clear();
    m_colors.clear();
    m_indexes.clear();
    m_reads.clear();
    m_visible.clear();
    m_selected.clear();
}

int GeneData::addQuad(const float x, const float y, const float size, const QColor& color)
{
    const unsigned int index_count = static_cast<unsigned int>(m_vertices.size());

    m_vertices.append(QVector3D(x - size / 2.0, y - size / 2.0, 0.0));
    m_vertices.append(QVector3D(x + size / 2.0, y - size / 2.0, 0.0));
    m_vertices.append(QVector3D(x + size / 2.0, y + size / 2.0, 0.0));
    m_vertices.append(QVector3D(x - size / 2.0, y + size / 2.0, 0.0));

    m_textures.append(ta);
    m_textures.append(tb);
    m_textures.append(tc);
    m_textures.append(td);

    const QVector4D opengl_color = fromQtColor(color);
    m_colors.append(opengl_color);
    m_colors.append(opengl_color);
    m_colors.append(opengl_color);
    m_colors.append(opengl_color);

    m_indexes.append(index_count);
    m_indexes.append(index_count + 1);
    m_indexes.append(index_count + 2);
    m_indexes.append(index_count);
    m_indexes.append(index_count + 2);
    m_indexes.append(index_count + 3);

    // update custom vertex arrays
    for (int i = 0; i < QUAD_SIZE; ++i) {
        m_reads.append(0);
        m_visible.append(false);
        m_selected.append(false);
    }

    // return first index of the quad created
    return index_count;
}

void GeneData::updateQuadSize(const int index, const float x, const float y, const float size)
{
    m_vertices[index] = QVector3D(x - size / 2.0, y - size / 2.0, 0.0);
    m_vertices[index + 1] = QVector3D(x + size / 2.0, y - size / 2.0, 0.0);
    m_vertices[index + 2] = QVector3D(x + size / 2.0, y + size / 2.0, 0.0);
    m_vertices[index + 3] = QVector3D(x - size / 2.0, y + size / 2.0, 0.0);
}

void GeneData::updateQuadColor(const int index, const QColor& color)
{
    const QVector4D opengl_color = fromQtColor(color);
    for (int i = 0; i < QUAD_SIZE; ++i) {
        m_colors[index + i] = opengl_color;
    }
}

void GeneData::updateQuadSelected(const int index, const bool selected)
{
    for (int i = 0; i < QUAD_SIZE; ++i) {
        m_selected[index + i] = selected;
    }
}

void GeneData::updateQuadVisible(const int index, const bool visible)
{
    for (int i = 0; i < QUAD_SIZE; ++i) {
        m_visible[index + i] = visible;
    }
}

void GeneData::updateQuadReads(const int index, const int reads)
{
    for (int i = 0; i < QUAD_SIZE; ++i) {
        m_reads[index + i] = reads;
    }
}

QColor GeneData::quadColor(const int index) const
{
    // all vertices has same color
    return fromOpenGLColor(m_colors.at(index));
}

bool GeneData::quadSelected(const int index) const
{
    // all vertices has same value
    return m_selected.at(index);
}

bool GeneData::quadVisible(const int index) const
{
    // all vertices has same value
    return m_visible.at(index);
}

int GeneData::quadReads(const int index) const
{
    // all vertices has same value
    return m_reads.at(index);
}

void GeneData::clearSelectionArray()
{
    std::fill(m_selected.begin(), m_selected.end(), false);
}
