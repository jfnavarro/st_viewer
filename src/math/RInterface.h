#ifndef RINTERFACE_H
#define RINTERFACE_H

#include <QString>
#include <QDebug>

//RcppArmadillo must be included before RInside
#include "RcppArmadillo.h"
#include "RInside.h"

namespace RInterface {

// Computes correlation betwee two vectors (method can be : pearson, spearman and kendall)
static float computeCorrelation(const std::vector<double> &A,
                                const std::vector<double> &B,
                                const std::string &method)
{
    RInside *R = nullptr;
    if (RInside::instancePtr() != nullptr) {
        R = RInside::instancePtr();
    } else {
        R = new RInside();
    }
    Q_ASSERT(A.size() == B.size());
    float corr = 0.0;
    try {
        (*R)["A"] = A;
        (*R)["B"] = B;
        (*R)["method"] = method;
        const std::string call = "corr = cor(A, B, method=method)";
        corr = Rcpp::as<float>(R->parseEval(call));
        qDebug() << "Computed R " << QString::fromStdString(method) << " correlation " << corr;
    } catch (const std::exception &e) {
        qDebug() << "Error computing R correlation " << e.what();
    }
    return corr;
}

// Classifies spots based on gene expression (tSNE + KMeans)
static void spotClassification(const mat &counts,
                               const int num_clusters,
                               const int inital_dim,
                               const int no_dims,
                               const int perplexity,
                               const int max_iter,
                               const double theta,
                               std::vector<int> &colors, mat &tsne)
{
    RInside *R = nullptr;
    if (RInside::instancePtr() != nullptr) {
        R = RInside::instancePtr();
    } else {
        R = new RInside();
    }
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
        const std::string call1 = "tsne_out = Rtsne(counts, dims=DIM,"
                "theta=theta, check_duplicates=FALSE, pca=TRUE,"
                "initial_dims=inital_dim, perplexity=perplexity,"
                "max_iter=max_iter, verbose=FALSE);"
                "tsne_out = tsne_out$Y[,1:DIM];";
        const std::string call2 = "fit = kmeans(tsne_out, k)$cluster";
        tsne = Rcpp::as<mat>(R->parseEval(call1));
        colors = Rcpp::as<std::vector<int>>(R->parseEval(call2));
        qDebug() << "Computed Spot colors " << colors.size();
        Q_ASSERT(colors.size() == counts.n_rows);
    } catch (const std::exception &e) {
        qDebug() << "Error doing R dimensionality reduction " << e.what();
    }
}

// Computes size factors using the DESEq2 method (one factor per spot)
static rowvec computeDESeqFactors(const mat &counts)
{
    RInside *R = nullptr;
    if (RInside::instancePtr() != nullptr) {
        R = RInside::instancePtr();
    } else {
        R = new RInside();
    }
    rowvec factors;
    factors.fill(1.0);
    try {
        const std::string R_libs = "suppressMessages(library(DESeq2));";
        R->parseEvalQ(R_libs);
        (*R)["counts"] = counts;
        // For DESeq2 genes must be rows so we transpose the matrix
        const std::string call = "dds = DESeq2::estimateSizeFactorsForMatrix(t(counts))";
        factors = Rcpp::as<rowvec>(R->parseEval(call));
        qDebug() << "Computed DESeq2 size factors " << factors.size();
        Q_ASSERT(factors.size() == counts.n_rows);
    } catch (const std::exception &e) {
        qDebug() << "Error computing DESeq2 size factors " << e.what();
    }
    return factors;
}

// Computes size factors using the SCRAN method (one factor per spot)
static rowvec computeScranFactors(const mat &counts)
{
    RInside *R = nullptr;
    if (RInside::instancePtr() != nullptr) {
        R = RInside::instancePtr();
    } else {
        R = new RInside();
    }

    rowvec factors;
    factors.fill(1.0);
    try {
        const std::string R_libs = "suppressMessages(library(scran))";
        R->parseEvalQ(R_libs);
        (*R)["counts"] = counts;
        // For Scran genes must be rows so we transpose the matrixs
        const std::string call = "counts = t(counts);"
                                 "num_spots = dim(counts)[2];"
                                 "sizes = vector(length=4);"
                                 "sizes[1] = floor((num_spots / 2) * 0.1);"
                                 "sizes[2] = floor((num_spots / 2) * 0.2);"
                                 "sizes[3] = floor((num_spots / 2) * 0.3);"
                                 "sizes[4] = floor((num_spots / 2) * 0.4);"
                                 "sce = newSCESet(countData=counts);"
                                 "clust = quickCluster(counts, min.size=sizes[1]);"
                                 "sce = computeSumFactors(sce, clusters=clust, positive=T, sizes=sizes);"
                                 "sce = normalize(sce);"
                                 "size_factors = sce@phenoData$size_factor";
        factors = Rcpp::as<rowvec>(R->parseEval(call));
        qDebug() << "Computed SCRAN size factors " << factors.size();
        Q_ASSERT(factors.size() == counts.n_rows);
    } catch (const std::exception &e) {
        qDebug() << "Error computing SCRAN size factors " << e.what();
    }
    return factors;
}

}
#endif // RINTERFACE_H
