/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/


#ifndef COMMON_H
#define COMMON_H

#include <QPointF>
#include <algorithm>
#include <QtCore/qmath.h>
#include <QSizeF>
#include <QColor>
#include <vector>

#include <cmath>

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

    if (clampSize.width() < min.width() || clampSize.height() < min.height()) {
        if (mode == Qt::IgnoreAspectRatio) {
            clampSize = clampSize.expandedTo(min);
        } else {
            clampSize = clampSize.scaled(min, Qt::KeepAspectRatioByExpanding);
        }
    }

    if (clampSize.width() > max.width() || clampSize.height() > max.height()) {
        if (mode == Qt::IgnoreAspectRatio) {
            clampSize = clampSize.boundedTo(max);
        } else {
            clampSize = clampSize.scaled(max, Qt::KeepAspectRatio);
        }
    }

    return clampSize;
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

//normalize v with min t0 and max t1 to
//lay in between [0-1]
template <typename T, typename R>
inline const R norm(const T v, const T t0, const T t1)
{
    const T vh = clamp(v, t0, t1);
    return R(vh - t0) / R(t1 - t0);
}

//assumes t1 - t0 is bigger than 0 and that t3 - t2 is bigger than 0 and that t0 <= v <= t1
//maps a value from a range (t0-t1) to a range (t2-t3)
template <typename T, typename R>
inline const R linearConversion(const T v, const T t0, const T t1, const T t2, const T t3)
{
    return  R(( (v - t0) / (t1 - t0) ) * (t3 - t2) + t2);
}

//normalize nv with min t0 and max t1
//assuming nv was normalized with range [0-1]
template <typename T, typename R>
inline const T denorm(const R nv, const T t0, const T t1)
{
    //should not be necessary to clamp to 0-1
    const R vh = clamp(nv, R(0.0), R(1.0));
    return T(vh * (t1 - t0)) + t0;
}

//linear interpolation between color c0 and color c1 given a value t
inline const QColor lerp(const qreal t, const QColor &c0, const QColor &c1)
{
    //TODO should do the interpolation in HSV space
    return QColor(
                (c0.red() + ((c1.red() - c0.red()) * t)),
                (c0.green() + ((c1.green() - c0.green()) * t)),
                (c0.blue() + ((c1.blue() - c0.blue()) * t)),
                (c0.alpha() + ((c1.alpha() - c0.alpha()) * t))
                );
}

//inverse linear interpolation between color c0 and color c1 given a value t
inline const QColor invlerp(const qreal t, const QColor &c0, const QColor &c1)
{
    //TODO should do the interpolation in HSV space
    const qreal invt = 1.0 / (1.0 - t);
    return QColor(
                (c0.red() - (t * c1.red())) * invt,
                (c0.green() - (t * c1.green())) * invt,
                (c0.blue() - (t * c1.blue())) * invt,
                (c0.alpha() - (t * c1.alpha())) * invt
                );
}


//Euclidean distance between two vectors of type T such that T has binary +,-,*
template<class T>
inline qreal euclidean(std::vector<T> v1, std::vector<T> v2){
    Q_ASSERT(v1.size() == v2.size());
    T diff;
    T sum;
    diff = v1[0] - v2[0];
    sum = diff * diff;
    for (unsigned int i = 1; i < v1.size(); ++i){
        diff = v1[i] - v2[i];
        sum += diff * diff;
    }

    return std::sqrt(static_cast<double>(sum));
}

//Jaccard Coefficient.	Use for asymetric binary values
template<class T>
inline qreal jaccard(const QVector<T> &v1,const QVector<T> &v2){
    Q_ASSERT(v1.size() == v2.size());
    int f11 = 0;
    int f00 = 0;
    for (unsigned int i = 0; i < v1.size(); ++i){
        if(v1[i] == v2[i]){
            if(v1[i]) {
                ++f11;
            } else {
                ++f00;
            }
        }
    }

    return static_cast<qreal>(f11)
            / static_cast<qreal>(v1.size() - (f11 + f00));
}

//The mean of a vector
template<class T>
inline qreal mean(const QVector<T> &v1){
    T sum = v1[0];
    for (int i = 1; i < v1.size(); ++i) {
        sum += v1[i];
    }

    return static_cast<qreal>(sum) / static_cast<qreal>(v1.size());
}

//The Covariance
template<class T>
inline qreal covariance(const QVector<T> &v1, const QVector<T> &v2) {
    Q_ASSERT(v1.size() == v2.size());
    const qreal mean1 = mean(v1);
    const qreal mean2 = mean(v2);
    qreal sum = 0.0;
    for (int i = 0; i < v1.size(); ++i){
        sum += (static_cast<qreal>(v1[i]) - mean1) * (static_cast<qreal>(v2[i]) - mean2);
    }

    return sum / static_cast<qreal>(v1.size() - 1);
}

//standard deviation the covariance where both vectors are the same.
template<class T>
inline qreal std_dev(const QVector<T> &v1){
    return sqrt(covariance(v1, v1));
}

//Pearson Correlation
template<class T>
inline qreal pearson(const QVector<T> &v1, const QVector<T> &v2) {
    if (std_dev(v1) * std_dev(v2) == 0){
        //a standard deviaton was 0...
        return -1;
    }

    return covariance(v1,v2) / (std_dev(v1) * std_dev(v2));
}

template<typename T>
inline QVector<T> logVectorValues(const QVector<T>& input)
{
    QVector<T> output;
    foreach(T value, input) {
        output.push_back(std::log1p(value));
    }

    return output;
}

template<typename T>
inline T tpmNormalization(const int reads, const int totalReads) {
    return static_cast<T>((reads * 10e6) / totalReads);
}

} // end name space

#endif // COMMON_H //
