#ifndef COMMON_H
#define COMMON_H

#include <QPointF>
#include <algorithm>
#include <QtCore/qmath.h>
#include <QSizeF>
#include <QColor>
#include <vector>

#include <cmath>
#include <armadillo>
#include "tsne.h"

using namespace arma;

constexpr double PI = 3.14159265358979323846;
constexpr double NUM_E = 2.71828182845904523536;

// Common provides miscellaneous functionality for maths and statistics
namespace STMath
{
// clamp size to
// NOTE: Qt::KeepAspectRatio might be prone to numerical errors
//(ie. any skewing introduced due to num error will be kept)
inline const QSizeF clamp(const QSizeF &size,
                          const QSizeF &min,
                          const QSizeF &max,
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

template <typename T>
inline T qMod(T x, T y)
{
    return x - y * static_cast<T>(qFloor(x / y));
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

// normalize v with min t0 and max t1 to lay in between [0-1]
template <typename T, typename R>
inline const R norm(const T v, const T t0, const T t1)
{
    const T vh = clamp(v, t0, t1);
    return R(vh - t0) / R(t1 - t0);
}

// assumes t1 - t0 is bigger than 0 and that t3 - t2 is bigger than 0 and that
// t0 <= v <= t1
// maps a value from a range (t0-t1) to a range (t2-t3)
template <typename T, typename R>
inline const R linearConversion(const T v, const T t0, const T t1, const T t2, const T t3)
{
    return R(((v - t0) / (t1 - t0)) * (t3 - t2) + t2);
}

// normalize nv with min t0 and max t1
// assuming nv was normalized with range [0-1]
template <typename T, typename R>
inline const T denorm(const R nv, const T t0, const T t1)
{
    // should not be necessary to clamp to 0-1
    const R vh = clamp(nv, R(0.0), R(1.0));
    return T(vh * (t1 - t0)) + t0;
}

// linear interpolation between color c0 and color c1 given a value t
inline const QColor lerp(const double t, const QColor &c0, const QColor &c1)
{
    // TODO should do the interpolation in HSV space
    return QColor::fromRgbF((c0.redF() + ((c1.redF() - c0.redF()) * t)),
                            (c0.greenF() + ((c1.greenF() - c0.greenF()) * t)),
                            (c0.blueF() + ((c1.blueF() - c0.blueF()) * t)),
                            (c0.alphaF() + ((c1.alphaF() - c0.alphaF()) * t)));
}

// inverse linear interpolation between color c0 and color c1 given a value t
inline const QColor invlerp(const double t, const QColor &c0, const QColor &c1)
{
    // TODO should do the interpolation in HSV space
    const double invt = 1.0 / (1.0 - t);
    return QColor::fromRgbF((c0.redF() - (t * c1.redF())) * invt,
                            (c0.greenF() - (t * c1.greenF())) * invt,
                            (c0.blueF() - (t * c1.blueF())) * invt,
                            (c0.alphaF() - (t * c1.alphaF())) * invt);
}

// Euclidean distance between two vectors of type T such that T has binary +,-,*
template <class T>
inline double euclidean(const std::vector<T> &v1, const std::vector<T> &v2)
{
    Q_ASSERT(v1.size() == v2.size());
    T diff;
    T sum;
    diff = v1[0] - v2[0];
    sum = diff * diff;
    for (size_t i = 1; i < v1.size(); ++i) {
        diff = v1[i] - v2[i];
        sum += diff * diff;
    }
    return std::sqrt(static_cast<double>(sum));
}

template <class T>
inline double euclidean(const T x1, const T y1, const T x2, const T y2)
{
    const T x = x1 - x2;
    const T y = y1 - y2;
    return std::sqrt(std::pow(x, 2) + std::pow(y, 2));
}

// The statistical mean of a vector of values
template <class T>
inline double mean(const std::vector<T> &v)
{
    const double sum = std::accumulate(v.begin(), v.end(), 0.0);
    return sum / v.size();
}

// The statistical standard deviation of a vector of values
template <class T>
inline double std_dev(const std::vector<T> &v)
{
    const double mean = meanVector(v);
    std::vector<double> diff(v.size());
    std::transform(v.begin(), v.end(), diff.begin(), [mean](double x) { return x - mean; });
    const double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
    return std::sqrt(sq_sum / v.size());
}

// The statistical standard deviation of two vectors of values
template <class T>
inline double covariance(const std::vector<T> &v1, const std::vector<T> &v2)
{
    const double mean1 = mean(v1);
    const double mean2 = mean(v2);
    double sum = 0;
    for (size_t i = 0; i < v1.size(); ++i) {
        sum += ((v1[i] - mean1) * (v2[i] - mean2));
    }
    return sum / (v1.size() - 1);
}

// Pearson Correlation
template <class T>
inline double pearson(const std::vector<T> &v1, const std::vector<T> &v2)
{
    const double std = std_dev(v1) * std_dev(v2);
    if (std == 0) {
        // a standard deviaton was 0...
        return -1;
    }

    return covariance(v1, v2) / std;
}

// Return the vector of log + 1 of the given vector
template <typename T>
inline std::vector<T> logVectorValues(const std::vector<T> &input)
{
    std::vector<T> output(input.size());
    std::transform(input.begin(), input.end(), std::back_inserter(output), std::log1p);
    return output;
}

// Simple pval correction using the Bonferroni method
//TODO implement the Benjamini/Hochberg method
inline std::vector<double> p_adjust(const std::vector<double> &pvals) {
    const double m = static_cast<double>(pvals.size());
    std::vector<double> padjust;
    for (auto pval : pvals) {
        padjust.push_back(pval / m);
    }
    return padjust;
}

inline mat PCA(const mat &data,
               const int no_dims,
               const bool center = false,
               const bool scale = false,
               const bool debug = false) {

    mat X = data;
    if (center) {
        const auto means = mean(X,0);
        X.each_row() -= means;
    }
    if (scale) {
        const auto std = stddev(X,0,0);
        X.each_row() /= std;
    }

    mat U;
    vec s;
    mat V;
    svd_econ(U, s, V, X);
    s = s.head(no_dims);
    U = U.head_cols(no_dims);
    // X_new = X * V = U * S * V^T * V = U * S
    U.each_row() %= s.t();
    if (debug) {
        std::cout << "PCA: " << U.n_rows << " - " << U.n_cols << std::endl;
        s.print();
    }
    return U;
}

inline mat kmeans_clustering(const mat &data,
                             const int k,
                             const bool debug = false) {
    mat centroids;
    const bool status = kmeans(centroids, data.t(), k, random_subset, 1000, false);
    if (debug) {
        std::cout << "k-means: " << status << " : " << centroids.n_rows << " - " << centroids.n_cols << std::endl;
        centroids.print();
    }
    return centroids;
}


inline mat tSNE(const mat &data,
                const double theta = 0.5,
                const int perplexity = 30,
                const int max_iter = 1000,
                const int no_dims = 2,
                const int init_dim = 50,
                const int rand_seed = -1,
                const bool debug = false) {

    const int N = data.n_rows;
    mat data_reduced = PCA(data, init_dim, true, false, false);
    double *Y = new double[N * no_dims];
    double *X = data_reduced.memptr();
    TSNE::run(X, N, init_dim, Y, no_dims,
              perplexity, theta, rand_seed, false, max_iter, 250, 250);
    mat manifold(N, no_dims);
    if (debug) {
        std::cout << "t-SNE: " << manifold.n_rows << " - " << manifold.n_cols << std::endl;
        manifold.print();
    }
    delete[](Y);
    Y = nullptr;
    return manifold;
}


inline double log_normal(const double x, const double m, const double s)
{
    if (x > 0) {
        return (1.0 / (s*x*std::sqrt(PI))) * std::exp(-std::pow(2, std::log(x)-m) / std::pow(2, 2*s));
    } else {
        return 0;
    }
}

inline double normal(const double x, const double m, const double s)
{
    if (x > 0) {
        return (1.0 / (s*std::sqrt(PI))) * std::exp(-0.5*(std::pow(2, (x-m)/s)));
    } else {
        return 0;
    }
}

inline double chi_squared(const double x, const double df)
{
    if (x > 0) {
        return (1.0 / (std::pow(2.0, 0.5*df) * std::tgamma(df*0.5))) * std::pow(x, 0.5*df-1) * std::exp(-0.5*x);
    } else {
        return 0;
    }
}

// Approximation of the lower incomplete gamma function
// obtained from https://github.com/brianmartens/BetaFunction/blob/master/BetaFunction/bmath.h
// s is the DF and z is X
inline double low_incomplete_gamma(const double s, const double z)
{
    const int MAXITER = 100;
    const double epsilon = .0000000001;
    int i = 1;
    double initial = (std::pow(z,s) * std::pow(NUM_E,-z)) / s; // z^0 == 1
    for (;;) {
        double denom = s;
        for(int j=1; j<=i; ++j) {
            denom = denom * (s+j);
        }
        double num = std::pow(z,s) * std::pow(NUM_E,-1*z) * std::pow(z,i);
        double test = num / denom;
        initial += test;
        ++i;
        if (std::abs(test) < epsilon || i >= MAXITER) {
            break;
        }
    }
    return initial;
}

inline double chi_squared_cdf(const double x, const double df)
{
    if (x > 0) {
        return low_incomplete_gamma(df/2, x/2) / std::tgamma(df/2);
    } else {
        return 0;
    }
}
} // end name space

#endif // COMMON_H //
