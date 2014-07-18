/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/


#ifndef COMMON_H
#define COMMON_H

#include <QPointF>
#include <QColor4ub>
#include <algorithm>
#include <QtCore/qmath.h>
#include <QSizeF>

// Common provides miscellaneous functionality related to the opengl library.
namespace STMath
{
// clamp size to
//NOTE: Qt::KeepAspectRatio might be prone to numerical errors
//(ie. any skewing introduced due to num error will be kept)
inline const QSizeF clamp(const QSizeF& size,
                          const QSizeF& min,
                          const QSizeF& max,
                          Qt::AspectRatioMode mode = Qt::IgnoreAspectRatio)
{
    QSizeF clampSize = size;

    if ((clampSize.width() < min.width()) || (clampSize.height() < min.height())) {
        if (mode == Qt::IgnoreAspectRatio) {
            clampSize = clampSize.expandedTo(min);
        } else {
            clampSize = clampSize.scaled(min, Qt::KeepAspectRatioByExpanding);
        }
    }

    if ((clampSize.width() > max.width()) || (clampSize.height() > max.height())) {
        if (mode == Qt::IgnoreAspectRatio) {
            clampSize = clampSize.boundedTo(max);
        } else {
            clampSize = clampSize.scaled(max, Qt::KeepAspectRatio);
        }
    }

    return clampSize;
}

inline int toGreyAverage(QRgb rgb)
{
    return (qRed(rgb) + qGreen(rgb) + qBlue(rgb)) / 3;
}

inline qreal qMod(qreal x, qreal y)
{
    return x - y * qFloor(x / y);
}

inline const QPointF min(const QPointF &a, const QPointF &b)
{
    return QPointF(std::min(a.x(), b.x()), std::min(a.y(), b.y()));
}

inline const QPointF max(const QPointF &a, const QPointF &b)
{
    return QPointF(std::max(a.x(), b.x()), std::max(a.y(), b.y()));
}

inline bool qFuzzyEqual(const QPointF &p0, const QPointF &p1)
{
    return qFuzzyCompare(p0.x(), p1.x()) && qFuzzyCompare(p0.y(), p1.y());
}

template <typename T>
inline T clamp(T in, T low, T high)
{
    return std::min(std::max(in, low), high);
}

template <typename T, typename R>
inline const R norm(const T v, const T t0, const T t1)
{
    const T vh = clamp(v, t0, t1);
    return R(vh - t0) / R(t1 - t0);
}

template <typename T, typename R>
inline const T denorm(const R nv, const T t0, const T t1)
{
    const R vh = clamp(nv, R(0.0), R(1.0));
    return T(vh * (t1 - t0)) + t0;
}

inline const QColor4ub lerp(const qreal t, const QColor4ub &c0, const QColor4ub &c1)
{
    return QColor4ub(
                (c0.red() + (c1.red() - c0.red()) * t),
                (c0.green() + (c1.green() - c0.green()) * t),
                (c0.blue() + (c1.blue() - c0.blue()) * t),
                (c0.alpha() + (c1.alpha() - c0.alpha()) * t)
                );
}

inline const QColor4ub invlerp(const qreal t, const QColor4ub &c0, const QColor4ub &c1)
{
    const qreal invt = 1.0 / (1.0 - t);
    return QColor4ub(
                (c0.red() - t * c1.red()) * invt,
                (c0.green() - t * c1.green()) * invt,
                (c0.blue() - t * c1.blue()) * invt,
                (c0.alpha() - t * c1.alpha()) * invt
                );
}

} // end name space

#endif // COMMON_H //
