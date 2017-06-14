#ifndef RINTERFACE_H
#define RINTERFACE_H

#include <QString>
#include <QDebug>

//RcppArmadillo must be included before RInside
#include "RcppArmadillo.h"
#include "RInside.h"

namespace RInterface {

typedef Mat<float> Matrix;
typedef Row<float> rowvec;

// Computes correlation betwee two vectors (method can be : pearson, spearman and kendall)
static float computeCorrelation(const std::vector<float> &A,
                                const std::vector<float> &B,
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

// Computes size factors using the DESEq2 method (one factor per spot)
static rowvec computeDESeqFactors(Matrix counts)
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
static rowvec computeScranFactors(const Matrix &counts)
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
                                 "sizes = vector(mode=\"numeric\", length=4);"
                                 "sizes[1] = (num_spots / 2) * 0.25;"
                                 "sizes[2] = (num_spots / 2) * 0.5;"
                                 "sizes[3] = (num_spots / 2) * 0.75;"
                                 "sizes[4] = (num_spots / 2);"
                                 "sce = newSCESet(countData=counts);"
                                 "clust = quickCluster(counts, min.size=sizes[1]);"
                                 "sce = computeSumFactors(sce, clusters=clust, positive=T, sizes=sizes);"
                                 "sce = normalize(sce, recompute_cpm=FALSE);"
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
