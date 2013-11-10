/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef QTGL_H
#define QTGL_H

#include <QString>

#ifndef QT_NO_DATASTREAM
#include <QDataStream>
#include <QDebug>
#endif // QT_NO_DATASTREAM //

#include <QPoint>
#include <QPointF>
#include <QRect>
#include <QRectF>
#include <QColor>
#include <QTransform>

#include "GLCommon.h"
#include "GLColor.h"
#include "math/GLAABB.h"
#include "math/GLMatrix.h"
#include "math/GLVector.h"

// GLQt implements convenience methods for converting between Qt specific
// and (our) GL specific objects.
namespace GL
{
// GLpoint
inline const GLpoint toGLpoint(qreal x, qreal y);
inline const GLpoint toGLpoint(const QPoint& point);
inline const GLpoint toGLpoint(const QPointF& point);

inline const GLpoint toGLpoint(const QSize& size);
inline const GLpoint toGLpoint(const QSizeF& size);

// GLrectangle
inline const GLrectangle toGLrectangle(const QRect& rect);
inline const GLrectangle toGLrectangle(const QRectF& rect);

// GLcolor
inline const GLcolor toGLcolor(const QColor& color);
inline const QColor toQColor(const GLcolor& color);

// GLmatrix
inline const GLmatrix toGLmatrix(const QTransform& transform);

// GLaabb
inline const GLaabb toGLaabb(const QRect& rect);
inline const GLaabb toGLaabb(const QRectF& rect);

// QString
inline QString toQString(const GLaabb &p);
inline QString toQString(const GLpoint &p);
inline QString toQString(const GLvector &v);
inline QString toQString(const GLcolor &c);

} // namespace GL //

#ifndef QT_NO_DATASTREAM

inline QDebug& operator<<(QDebug& stream, const GL::GLaabb& b);
inline QDebug& operator<<(QDebug& stream, const GL::GLpoint& p);
inline QDebug& operator<<(QDebug& stream, const GL::GLvector& v);
inline QDebug& operator<<(QDebug& stream, const GL::GLcolor& c);

#endif // QT_NO_DATASTREAM //

/****************************************** DEFINITION ******************************************/

namespace GL
{
// GLpoint
const GLpoint toGLpoint(qreal x, qreal y)
{
    const GLfloat fx = static_cast<GLfloat>(x);
    const GLfloat fy = static_cast<GLfloat>(y);
    return GLpoint(fx, fy);
}
const GLpoint toGLpoint(const QPoint& point)
{
    const GLfloat x = static_cast<GLfloat>(point.x());
    const GLfloat y = static_cast<GLfloat>(point.y());
    return GLpoint(x, y);
}
const GLpoint toGLpoint(const QPointF& point)
{
    const GLfloat x = static_cast<GLfloat>(point.x());
    const GLfloat y = static_cast<GLfloat>(point.y());
    return GLpoint(x, y);
}

inline const GLpoint toGLpoint(const QSize& size)
{
    const GLfloat width = static_cast<GLfloat>(size.width());
    const GLfloat height = static_cast<GLfloat>(size.height());
    return GLpoint(width, height);
}
inline const GLpoint toGLpoint(const QSizeF& size)
{
    const GLfloat width = static_cast<GLfloat>(size.width());
    const GLfloat height = static_cast<GLfloat>(size.height());
    return GLpoint(width, height);
}

// GLrectangle
const GLrectangle toGLrectangle(const QRect& rect)
{
    return GLrectangle(
                toGLpoint(rect.topLeft()),
                toGLpoint(rect.topRight()),
                toGLpoint(rect.bottomRight()),
                toGLpoint(rect.bottomLeft())
                );
}
const GLrectangle toGLrectangle(const QRectF& rect)
{
    return GLrectangle(
                toGLpoint(rect.topLeft()),
                toGLpoint(rect.topRight()),
                toGLpoint(rect.bottomRight()),
                toGLpoint(rect.bottomLeft())
                );
}

// GLcolor
const GLcolor toGLcolor(const QColor& color)
{
    const GLfloat red = static_cast<GLfloat>(color.redF());
    const GLfloat green = static_cast<GLfloat>(color.greenF());
    const GLfloat blue = static_cast<GLfloat>(color.blueF());
    const GLfloat alpha = static_cast<GLfloat>(color.alphaF());
    return GLcolor(red, green, blue, alpha);
}
const QColor toQColor(const GLcolor& color)
{
    return QColor::fromRgbF(
                static_cast<qreal>(color.red),
                static_cast<qreal>(color.green),
                static_cast<qreal>(color.blue),
                static_cast<qreal>(color.alpha)
                );
}

// GLmatrix
const GLmatrix toGLmatrix(const QTransform& transform)
{
    return GLmatrix(
                transform.m11(), transform.m12(), transform.m13(),
                transform.m21(), transform.m22(), transform.m23(),
                transform.m31(), transform.m32(), transform.m33());
}

// GLaabb
inline const GLaabb toGLaabb(const QRect& rect)
{
    return GLaabb(
                toGLpoint(rect.topLeft()),
                toGLpoint(rect.size())
                );
}
inline const GLaabb toGLaabb(const QRectF& rect)
{
    return GLaabb(
                toGLpoint(rect.topLeft()),
                toGLpoint(rect.size())
                );
}

// QString
inline QString toQString(const GLaabb &b)
{
    return QString("(x:%1, y:%2, w:%3, h:%4)").arg(b.x).arg(b.y).arg(b.width).arg(b.height);
}
inline QString toQString(const GLpoint &p)
{
    return QString("(x:%1, y:%2)").arg(p.x).arg(p.y);
}
inline QString toQString(const GLvector &v)
{
    return QString("[x:%1, y:%2]").arg(v.x).arg(v.y);
}
inline QString toQString(const GLcolor &c)
{
    return QString("[r:%1, g:%2, b:%3, a:%4]").arg(c.red).arg(c.green).arg(c.blue).arg(c.alpha);
}

} // namespace GL //

#ifndef QT_NO_DATASTREAM

QDebug& operator<<(QDebug& stream, const GL::GLaabb& b)
{
    return stream << GL::toQString(b);
}
QDebug& operator<<(QDebug& stream, const GL::GLpoint& p)
{
    return stream << GL::toQString(p);
}
QDebug& operator<<(QDebug& stream, const GL::GLvector& v)
{
    return stream << GL::toQString(v);
}
QDebug& operator<<(QDebug& stream, const GL::GLcolor& c)
{
    return stream << GL::toQString(c);
}

#endif // QT_NO_DATASTREAM //

#endif // QTGL_H //
