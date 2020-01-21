#ifndef COMMON_H
#define COMMON_H

#include <QPointF>
#include <algorithm>
#include <QtCore/qmath.h>
#include <QSizeF>
#include <QColor>
#include <vector>

#include <cmath>
#include <algorithm>
#include <functional>
#include <queue>
#include <armadillo>
#include "tsne.h"

using namespace arma;

// Common provides miscellaneous functionality for maths and statistics
namespace STMath
{

constexpr double PI = 3.14159265358979323846;
constexpr double NUM_E = 2.71828182845904523536;

inline double deg3rad(const double degrees)
{
    return (degrees * PI) / 180;
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
    const double m = mean(v);
    std::vector<double> diff(v.size());
    std::transform(v.begin(), v.end(), diff.begin(), [m](double x) { return x - m; });
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

template <typename T>
inline std::vector<size_t> sort_indexes(const std::vector<T> &v)
{
  std::vector<size_t> idx(v.size());
  std::iota(idx.begin(), idx.end(), 0);
  std::stable_sort(idx.begin(), idx.end(),
       [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});
  return idx;
}

// Simple pval correction using the Bonferroni method
inline std::vector<double> p_adjustB(const std::vector<double> &pvals)
{
    const double m = static_cast<double>(pvals.size());
    std::vector<double> padjust;
    for (auto pval : pvals) {
        padjust.push_back(pval / m);
    }
    return padjust;
}

// Simple pval correction using the Bonferroni-Holm method
inline std::vector<double> p_adjustBH(const std::vector<double> &pvals)
{
    const size_t n = pvals.size();
    const std::vector<size_t> idx = sort_indexes(pvals);
    std::vector<double> pvals_copy(pvals);
    std::sort(pvals_copy.begin(), pvals_copy.end());
    std::vector<double> adj_pvals(n);
    for (size_t rank = 0; rank != pvals_copy.size(); ++rank) {
        const double pvalue = pvals_copy.at(rank);
        const size_t i = idx.at(rank);
        adj_pvals.at(i) = static_cast<double>(n - rank) * pvalue;
    }
    return adj_pvals;
}

// Simple pval correction using the Benjamini-Hochberg method
inline std::vector<double> p_adjustBenH(const std::vector<double> &pvals)
{
    const size_t n = pvals.size();
    std::vector<size_t> idx = sort_indexes(pvals);
    std::reverse(idx.begin(), idx.end());
    std::vector<double> pvals_copy(pvals);
    std::sort(pvals_copy.begin(), pvals_copy.end());
    std::reverse(pvals_copy.begin(), pvals_copy.end());
    std::vector<double> new_values;
    std::vector<double> adj_pvals(n);
    for (size_t i = 0; i != pvals_copy.size(); ++i) {
        const size_t rank = n - i;
        const double pvalue = pvals_copy.at(i);
        new_values.push_back((n/rank) * pvalue);
    }
    for (size_t i = 0; i != n-1; ++i) {
        if (new_values.at(i) < new_values.at(i+1)) {
            new_values.at(i+1) = new_values.at(i);
        }
    }
    for (size_t i = 0; i != pvals_copy.size(); ++i) {
        const size_t ix = idx.at(i);
        adj_pvals.at(ix) = new_values.at(i);
    }
    return adj_pvals;
}

inline mat PCA(const mat &data,
               const int no_dims,
               const bool center = false,
               const bool scale = false,
               const bool debug = false)
{
    mat X = data;
    if (center) {
        const auto means = mean(X,0);
        X.each_row() -= means;
    }
    if (scale) {
        const auto std = stddev(X,0,0);
        X.each_row() /= std;
    }

    mat coeff;
    mat score;
    vec latent;
    vec tsquared;
    princomp(coeff, score, latent, tsquared, X);
    if (debug) {
        std::cout << "PCA: " << score.n_rows << " - " << score.n_cols << std::endl;
        latent.print();
    }
    return score.head_cols(no_dims);
}

inline mat kmeans_clustering(const mat &data,
                             const int k,
                             const bool debug = false)
{
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
                const bool debug = false)
{
    const int N = data.n_rows;
    mat data_reduced = PCA(data, init_dim, true, false, false);
    double *Y = new double[N * no_dims * sizeof(double)];
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

inline double normal_cdf(const double x)
{
    const double A1 = 0.31938153;
    const double A2 = -0.356563782;
    const double A3 = 1.781477937;
    const double A4 = -1.821255978;
    const double A5 = 1.330274429;
    const double RSQRT2PI = 0.39894228040143267793994605993438;

    const double K = 1.0 / (1.0 + 0.2316419 * std::fabs(x));

    const double cnd = RSQRT2PI * std::exp(- 0.5 * x * x) *
            (K * (A1 + K * (A2 + K * (A3 + K * (A4 + K * A5)))));

    return x > 0 ? 1-cnd : cnd;
}

// Wilcoxon rigned rank test (as implemented in Scipy.stats)
inline double wilcoxon_rank_test(const uvec &a, const uvec &b)
{
    const size_t n1 = a.size();
    const size_t n2 = b.size();
    const uvec all = join_cols(a,b);
    const uvec ranked = arma::sort_index(arma::sort_index(all)) + 1;
    const uvec x = ranked.head(n1);
    const double s = sum(x);
    const double expected = n1 * (n1 + n2 +1) / 2.0;
    const double z = (s - expected) / std::sqrt(n1 * n2 * (n1+ n2 + 1) / 12.0);
    // survival function = 1 - cdf
    const double prob = 2 * (1 - normal_cdf(std::fabs(z)));
    return prob;
}

} // end name space

#endif // COMMON_H //
