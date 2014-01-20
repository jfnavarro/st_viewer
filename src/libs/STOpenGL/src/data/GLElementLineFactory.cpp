#include "GLElementLineFactory.h"

#include <QLineF>
#include <QPointF>

namespace GL
{

GLElementLineFactory::GLElementLineFactory(GLElementData &data, GLflag flags)
    : GLElementShapeFactory(data, flags)
{
    // initialize data
    m_data->setMode(GL_LINES);
}

GLElementLineFactory::~GLElementLineFactory()
{

}

GLindex GLElementLineFactory::addShape(const QPointF &point1, const QPointF &point2)
{
    return addShape(QLineF(point1, point2));
}

GLindex GLElementLineFactory::addShape(const QLineF &line)
{
    GLindex index = m_data->addShape(line);

    if (m_flags & AutoAddColor) {
        m_data->addColorLine(m_color);
    }
    if (m_flags & AutoAddConnection) {
        m_data->connectLine(index);
    }

    return translateInternalIndex(index);
}

GLindex GLElementLineFactory::connect(const GLindex &externalIndex)
{
    const GLindex indexIn = translateExternalIndex(externalIndex);
    GLindex IndexOut;
    m_data->connectLine(indexIn, &IndexOut);
    return translateInternalIndex(IndexOut);
}

void GLElementLineFactory::deconnect(const GLindex &externalIndex)
{
    const GLindex internalIndex = translateExternalIndex(externalIndex);
    m_data->deconnectLine(internalIndex);
}

void GLElementLineFactory::setColor(const QColor4ub &color)
{
    m_color = color;
}

void GLElementLineFactory::setSize(const qreal &size)
{
    m_size = QPointF(size, size);
}

void GLElementLineFactory::setSize(const qreal &width, const qreal &height)
{
    m_size = QPointF(width, height);
}

GLindex GLElementLineFactory::size() const
{
    const GLElementData::IndexesType &indicies = m_data->indices();
    return translateInternalIndex( indicies.size() );
}

void GLElementLineFactory::setShape(const GLindex &index, const QLineF &line)
{
    const GLindex internalIndex = translateExternalIndex(index);
    m_data->setShape(internalIndex, line);
}

void GLElementLineFactory::setColor(const GLindex &index, const QColor4ub &color)
{
    const GLindex internalIndex = translateExternalIndex(index);
    m_data->setColorLine(internalIndex, color);
}

const QColor4ub GLElementLineFactory::getColor(const GLindex &index) const
{
    const GLindex internalIndex = translateExternalIndex(index);
    return m_data->getColor(internalIndex); // assume colors are reflected over all vertices
}

GLindex GLElementLineFactory::translateInternalIndex(const GLindex &internalIndex) const
{
    Q_ASSERT(internalIndex % size_value == 0);
    return internalIndex / size_value;
}

GLindex GLElementLineFactory::translateExternalIndex(const GLindex &externalIndex) const
{
    return externalIndex * size_value;
}

} //name space GL
