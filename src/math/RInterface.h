#ifndef RINTERFACE_H
#define RINTERFACE_H

#include <string>
#include <QDebug>

//RcppArmadillo must be included before RInside
#include "RcppArmadillo.h"
#include "RInside.h"

#include "viewPages/SettingsWidget.h"

namespace RInterface {

// Computes correlation between two vectors (method can be : pearson, spearman and kendall)
static double computeCorrelation(const std::vector<double> &A,
                                 const std::vector<double> &B,
                                 const std::string &method)
{
    RInside *R = RInside::instancePtr();
    Q_ASSERT(R != nullptr);
    Q_ASSERT(A.size() == B.size());
    double corr = -1.0;
    try {
        (*R)["A"] = A;
        (*R)["B"] = B;
        (*R)["method"] = method;
        const std::string call = "corr = cor(A, B, method=method)";
        corr = Rcpp::as<double>(R->parseEval(call));
        qDebug() << "Computed R " << QString::fromStdString(method) << " correlation " << corr;
    } catch (const std::exception &e) {
        qDebug() << "Error computing R correlation " << e.what();
        return corr;
    } catch (...) {
        qDebug() << "Unknown error computing R correlation";
        return corr;
    }
    return corr;
}

// Performs a grid interpolation between two set of points
static std::vector<unsigned> computeInterpolation(const std::vector<double> &x1,
                                                  const std::vector<double> &y1,
                                                  const std::vector<double> &x2,
                                                  const std::vector<double> &y2,
                                                  const std::vector<unsigned> &values)
{
    RInside *R = RInside::instancePtr();
    Q_ASSERT(R != nullptr);
    Q_ASSERT(x1.size() == y1.size());
    Q_ASSERT(x1.size() == values.size());
    Q_ASSERT(x2.size() == y2.size());
    std::vector<unsigned> results;
    try {
        const std::string R_libs = "suppressMessages(library(akima));";
        R->parseEvalQ(R_libs);
        (*R)["x1"] = x1;
        (*R)["y1"] = y1;
        (*R)["x2"] = x2;
        (*R)["y2"] = y2;
        (*R)["z"] = values;
        const std::string call = "s = interp(x1, y1, z, x2, y2)$z;";
        results = Rcpp::as<std::vector<unsigned>>(R->parseEval(call));
        Q_ASSERT(results.size() == x2.size());
        qDebug() << "Computed R Interpolation. In " << x1.size() << " Out " << x2.size();
    } catch (const std::exception &e) {
        qDebug() << "Error computing R Interpolation " << e.what();
        return results;
    } catch (...) {
        qDebug() << "Unknown error computing R Interpolation";
        return results;
    }
    return results;
}

// Computes a DEA (Differential Expression Analysis with DESeq2) between two selections
static void computeDEA(const mat &data,
                       const std::vector<std::string> &dataRows,
                       const std::vector<std::string> &dataCols,
                       const std::vector<std::string> &condition,
                       const SettingsWidget::NormalizationMode &normalization,
                       mat &results,
                       std::vector<std::string> &rows,
                       std::vector<std::string> &cols)
{
    RInside *R = RInside::instancePtr();
    Q_ASSERT(R != nullptr);
    try {
        const std::string R_libs = "suppressMessages(library(BiocParallel));"
                                   "register(MulticoreParam(4));"
                                   "suppressMessages(library(DESeq2));"
                                   "suppressMessages(library(scran))";
        R->parseEvalQ(R_libs);
        (*R)["counts"] = data;
        (*R)["rows"] = dataRows;
        (*R)["cols"] = dataCols;
        (*R)["condition"] = condition;
        std::string call = "exp_values = as.matrix(t(counts));"
                           "exp_values[is.na(exp_values)] = 0;"
                           "exp_values[exp_values < 0] = 0;"
                           "exp_values = apply(exp_values, c(1,2), as.numeric);"
                           "rownames(exp_values) = cols;";
        if (normalization == SettingsWidget::NormalizationMode::DESEQ) {
            call += "dds = DESeqDataSetFromMatrix(countData=exp_values, "
                    "colData=data.frame(condition=condition), design= ~ condition);";
        } else {
            call += "num_spots = dim(exp_values)[2];"
                    "sizes = vector(length=4);"
                    "sizes[1] = ceiling((num_spots / 2) * 0.1);"
                    "sizes[2] = ceiling((num_spots / 2) * 0.2);"
                    "sizes[3] = ceiling((num_spots / 2) * 0.3);"
                    "sizes[4] = ceiling((num_spots / 2) * 0.4);"
                    "sce = newSCESet(countData=exp_values);"
                    "sce = computeSumFactors(sce, positive=T, sizes=unique(sizes));"
                    "sce = normalize(sce);"
                    "dds = convertTo(sce, type='DESeq2');"
                    "colData(dds)$condition = as.factor(condition);"
                    "design(dds) = formula( ~ condition);";
        }
        call += "dds = DESeq(dds, fitType='mean', parallel=F);"
                "res = na.omit(results(dds, contrast=c('condition', 'A', 'B')));"
                "res = res[order(res$padj),];";
        const std::string call2 = "as.matrix(res);";
        const std::string call3 = "colnames(res);";
        const std::string call4 = "rownames(res);";
        R->parseEvalQ(call);
        results = Rcpp::as<mat>(R->parseEval(call2));
        cols = Rcpp::as<std::vector<std::string>>(R->parseEval(call3));
        rows = Rcpp::as<std::vector<std::string>>(R->parseEval(call4));
        qDebug() << "Computed R DEA with DESEq2";
    } catch (const std::exception &e) {
        qDebug() << "Error computing R DEA with DESEq2" << e.what();
    } catch (...) {
        qDebug() << "Unknown error computing R DEA with DESeq2";
    }
}

// Simply computes a PCA for the given matrix of counts
static void PCA(const mat &counts,
                const bool scale,
                const bool center,
                mat &results)
{
    RInside *R = RInside::instancePtr();
    Q_ASSERT(R != nullptr);
    try {
        (*R)["counts"] = counts;
        (*R)["scale"] = scale;
        (*R)["center"] = center;
        const std::string call = "pcs = prcomp(counts, center=center, scale.=scale);"
                                 "out = predict(pcs);"
                                 "out[,1:2];";
        results = Rcpp::as<mat>(R->parseEval(call));
        qDebug() << "Computed PCA " << results.n_rows;
        Q_ASSERT(results.n_rows == counts.n_rows);
    } catch (const std::exception &e) {
        qDebug() << "Error doing R PCA " << e.what();
    } catch (...) {
        qDebug() << "Unknown error computing R PCA";
    }
}

// Classifies spots based on gene expression (tSNE or PCA + KMeans or HClust)
static void spotClassification(const mat &counts,
                               const bool tsne,
                               const bool kmeans,
                               const int num_clusters,
                               const int inital_dim,
                               const int no_dims,
                               const int perplexity,
                               const int max_iter,
                               const double theta,
                               const bool scale,
                               const bool center,
                               std::vector<int> &colors,
                               mat &results)
{
    RInside *R = RInside::instancePtr();
    Q_ASSERT(R != nullptr);
    try {
        const std::string R_libs = "suppressMessages(library(Rtsne));";
        R->parseEvalQ(R_libs);
        (*R)["counts"] = counts;
        (*R)["k"] = num_clusters;
        (*R)["DIM"] = no_dims;
        (*R)["inital_dim"] = inital_dim;
        (*R)["perplexity"] = perplexity;
        (*R)["max_iter"] = max_iter;
        (*R)["theta"] = theta;
        (*R)["do_tsne"] = tsne;
        (*R)["do_kmeans"] = kmeans;
        (*R)["scale"] = scale;
        (*R)["center"] = center;
        const std::string call1 = "if (do_tsne) {"
                                  "    tsne_out = Rtsne(counts, dims=DIM,"
                                  "      theta=theta, check_duplicates=FALSE, pca=TRUE,"
                                  "      initial_dims=inital_dim, perplexity=perplexity,"
                                  "      max_iter=max_iter, verbose=FALSE);"
                                  "    tsne_out = tsne_out$Y[,1:DIM];"
                                  "} else {"
                                  "    pcs = prcomp(counts, center=center, scale.=scale);"
                                  "    tsne_out = predict(pcs);"
                                  "    tsne_out = tsne_out[,1:DIM];"
                                  "}";
        const std::string call2 = "if (do_kmeans) {\n"
                                  "    fit = kmeans(tsne_out, k)$cluster;"
                                  "} else {\n"
                                  "    h = hclust(dist(tsne_out), method='ward.D2');\n"
                                  "    fit = cutree(h, k=k);\n"
                                  "}\n"
                                  "if (!0 %in% fit) {\n"
                                  "    fit = fit - 1;\n"
                                  "}";
        results = Rcpp::as<mat>(R->parseEval(call1));
        colors = Rcpp::as<std::vector<int>>(R->parseEval(call2));
        qDebug() << "Computed Spot colors " << colors.size();
        Q_ASSERT(colors.size() == counts.n_rows);
    } catch (const std::exception &e) {
        qDebug() << "Error doing R dimensionality reduction " << e.what();
    } catch (...) {
        qDebug() << "Unknown error computing R dimensionality reduction";
    }
}

// Estimates an approximate number of spot classes (different spots types based on gene expression)
static unsigned computeSpotClasses(const mat &counts)
{
    RInside *R = RInside::instancePtr();
    Q_ASSERT(R != nullptr);
    Q_ASSERT(!counts.empty());
    unsigned clusters = 0;
    try {
        const std::string R_libs = "suppressMessages(library(BiocParallel));"
                                   "register(MulticoreParam(4));"
                                   "suppressMessages(library(scran))";
        R->parseEvalQ(R_libs);
        (*R)["counts"] = counts;
        const std::string call = "clusters = quickCluster(as.matrix(t(counts)), min.size=dim(counts)[1] / 10);"
                                 "clusters = length(unique(clusters[clusters != 0]))";
        clusters = Rcpp::as<double>(R->parseEval(call));
        qDebug() << "Computed R clusters with quickClust " << clusters;
    } catch (const std::exception &e) {
        qDebug() << "Error computing R clusters with quickClust " << e.what();
        return clusters;
    } catch (...) {
        qDebug() << "Unknown error computing R clusters with quickClust";
        return clusters;
    }
    return clusters;
}

// Computes size factors using the DESEq2 method (one factor per spot)
static rowvec computeDESeqFactors(const mat &counts)
{
    RInside *R = RInside::instancePtr();
    Q_ASSERT(R != nullptr);
    rowvec factors(counts.n_rows);
    factors.fill(1.0);
    try {
        const std::string R_libs = "suppressMessages(library(BiocParallel));"
                                   "register(MulticoreParam(4));"
                                   "suppressMessages(library(DESeq2));";
        R->parseEvalQ(R_libs);
        (*R)["counts"] = counts;
        // For DESeq2 genes must be rows so we transpose the matrix
        const std::string call = "dds = DESeq2::estimateSizeFactorsForMatrix(t(counts))";
        factors = Rcpp::as<rowvec>(R->parseEval(call));
        qDebug() << "Computed DESeq2 size factors " << factors.size();
        Q_ASSERT(factors.size() == counts.n_rows);
        if (!factors.is_finite()) {
            qDebug() << "Computed DESeq2 factors has non finite elements";
            factors.replace(datum::inf, 1.0);
            factors.replace(datum::nan, 1.0);
        }
        if (any(factors <= 0)) {
            qDebug() << "Computed DESeq2 factors has elements with zeroes or negative";
            factors.replace(0.0, 1.0);
        }
    } catch (const std::exception &e) {
        qDebug() << "Error computing DESeq2 size factors " << e.what();
        return factors;
    } catch (...) {
        qDebug() << "Unknown error computing DESeq2 size factors";
        return factors;
    }
    return factors;
}

// Computes size factors using the SCRAN method (one factor per spot)
static rowvec computeScranFactors(const mat &counts, const bool do_cluster)
{
    Q_UNUSED(do_cluster);
    RInside *R = RInside::instancePtr();
    Q_ASSERT(R != nullptr);
    rowvec factors(counts.n_rows);
    factors.fill(1.0);
    try {
        const std::string R_libs = "suppressMessages(library(scran))";
        R->parseEvalQ(R_libs);
        (*R)["counts"] = counts;
        // For Scran genes must be rows so we transpose the matrixs
        const std::string call = "counts = t(counts);"
                                 "num_spots = dim(counts)[2];"
                                 "sizes = vector(length=4);"
                                 "sizes[1] = ceiling((num_spots / 2) * 0.1);"
                                 "sizes[2] = ceiling((num_spots / 2) * 0.2);"
                                 "sizes[3] = ceiling((num_spots / 2) * 0.3);"
                                 "sizes[4] = ceiling((num_spots / 2) * 0.4);"
                                 "sce = newSCESet(countData=counts);"
                                 "sce = computeSumFactors(sce, positive=T, sizes=unique(sizes));"
                                 "sce = normalize(sce);"
                                 "size_factors = sce@phenoData$size_factor";
        factors = Rcpp::as<rowvec>(R->parseEval(call));
        qDebug() << "Computed SCRAN size factors " << factors.size();
        Q_ASSERT(factors.size() == counts.n_rows);
        if (!factors.is_finite()) {
            qDebug() << "Computed SCRAN factors has non finite elements";
            factors.replace(datum::inf, 1.0);
            factors.replace(datum::nan, 1.0);
        }
        if (any(factors <= 0)) {
            qDebug() << "Computed SCRAN factors has elements with zeroes or negative";
            factors.replace(0.0, 1.0);
        }
    } catch (const std::exception &e) {
        qDebug() << "Error computing SCRAN size factors " << e.what();
        return factors;
    } catch (...) {
        qDebug() << "Unknown error computing SCRAN size factors";
        return factors;
    }
    return factors;
}

}
#endif // RINTERFACE_H
