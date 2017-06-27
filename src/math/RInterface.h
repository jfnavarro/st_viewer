#ifndef RINTERFACE_H
#define RINTERFACE_H

#include <string>
#include <QDebug>

//RcppArmadillo must be included before RInside
#include "RcppArmadillo.h"
#include "RInside.h"

#include "viewPages/SettingsWidget.h"

namespace RInterface {

// Computes correlation betwee two vectors (method can be : pearson, spearman and kendall)
static double computeCorrelation(const std::vector<double> &A,
                                const std::vector<double> &B,
                                const std::string &method)
{
    RInside *R = RInside::instancePtr();
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
    } catch (...) {
        qDebug() << "Uknown error computing R correlation";
    }
    return corr;
}

// Computes a DEA (Differential Expression Analysis with DESeq2) between two selections
static void computeDEA(const mat &countsA,
                       const mat &countsB,
                       const std::vector<std::string> &rowsA,
                       const std::vector<std::string> &rowsB,
                       const std::vector<std::string> &colsA,
                       const std::vector<std::string> &colsB,
                       const SettingsWidget::NormalizationMode &normalization,
                       mat &results,
                       std::vector<std::string> &rows,
                       std::vector<std::string> &cols)
{
    RInside *R = RInside::instancePtr();

    try {
        const std::string R_libs = "suppressMessages(library(DESeq2));"
                                   "suppressMessages(library(plyr));"
                                   "suppressMessages(library(scran))";
        R->parseEvalQ(R_libs);

        (*R)["A"] = countsA;
        (*R)["B"] = countsB;
        (*R)["rowsA"] = rowsA;
        (*R)["rowsB"] = rowsB;
        (*R)["colsA"] = colsA;
        (*R)["colsB"] = colsB;
        std::string call = "A = as.data.frame(A); rownames(A) = rowsA; colnames(A) = colsA;"
                           "B = as.data.frame(B); rownames(B) = rowsB; colnames(B) = colsB;"
                           "merged = suppressMessages(join(A, B, by=NULL, type='full', match='all'));"
                           "rownames(merged) = c(rownames(A), rownames(B));"
                           "exp_values = t(merged);"
                           "exp_values[is.na(exp_values)] = 0;"
                           "exp_values = apply(exp_values, c(1,2), as.numeric);";
        if (normalization == SettingsWidget::NormalizationMode::DESEQ) {
            call += "size_factors = estimateSizeFactorsForMatrix(exp_values);";
        } else {
            call += "num_spots = dim(exp_values)[2];"
                    "sizes = vector(length=4);"
                    "sizes[1] = floor((num_spots / 2) * 0.1);"
                    "sizes[2] = floor((num_spots / 2) * 0.2);"
                    "sizes[3] = floor((num_spots / 2) * 0.3);"
                    "sizes[4] = floor((num_spots / 2) * 0.4);"
                    "sce = newSCESet(countData=exp_values);"
                    "sce = computeSumFactors(sce, positive=T, sizes=unique(sizes));"
                    "sce = normalize(sce);"
                    "size_factors = sce@phenoData$size_factor";
        }
        call +=
                "condition = c(rep('A', length(rowsA)), rep('B', length(rowsB)));"
                "coldata = data.frame(row.names=colnames(exp_values), condition=condition);"
                "dds = DESeqDataSetFromMatrix(countData=exp_values, colData=coldata, design=~condition);"
                "dds@colData$sizeFactor = size_factors;"
                "dds = estimateDispersions(dds, fitType='local');"
                "dds = nbinomWaldTest(dds);"
                "res = results(dds, contrast=c('condition', 'A', 'B'));";
        const std::string call2 = "values = as.matrix(res);";
        const std::string call3 = "cols = colnames(res);";
        const std::string call4 = "rows = rownames(res);";
        R->parseEvalQ(call);
        results = Rcpp::as<mat>(R->parseEval(call2));
        cols = Rcpp::as<std::vector<std::string>>(R->parseEval(call3));
        rows = Rcpp::as<std::vector<std::string>>(R->parseEval(call4));
        qDebug() << "Computed R DEA with DESEq2";
    } catch (const std::exception &e) {
        qDebug() << "Error computing R DEA with DESEq2" << e.what();
    } catch (...) {
        qDebug() << "Uknown error computing R DEA with DESeq2";
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
        qDebug() << "Uknown error computing R dimensionality reduction";
    }
}

// Computes size factors using the DESEq2 method (one factor per spot)
static rowvec computeDESeqFactors(const mat &counts)
{
    RInside *R = RInside::instancePtr();
    rowvec factors(counts.n_rows);
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
    } catch (...) {
        qDebug() << "Uknown error computing DESeq2 size factors";
    }
    return factors;
}

// Computes size factors using the SCRAN method (one factor per spot)
static rowvec computeScranFactors(const mat &counts)
{
    RInside *R = RInside::instancePtr();
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
                                 "sizes[1] = floor((num_spots / 2) * 0.1);"
                                 "sizes[2] = floor((num_spots / 2) * 0.2);"
                                 "sizes[3] = floor((num_spots / 2) * 0.3);"
                                 "sizes[4] = floor((num_spots / 2) * 0.4);"
                                 "sce = newSCESet(countData=counts);"
                                 "sce = computeSumFactors(sce, positive=T, sizes=unique(sizes));"
                                 "sce = normalize(sce);"
                                 "size_factors = sce@phenoData$size_factor";
        factors = Rcpp::as<rowvec>(R->parseEval(call));
        qDebug() << "Computed SCRAN size factors " << factors.size();
        Q_ASSERT(factors.size() == counts.n_rows);
    } catch (const std::exception &e) {
        qDebug() << "Error computing SCRAN size factors " << e.what();
    } catch (...) {
        qDebug() << "Uknown error computing SCRAN size factors";
    }
    return factors;
}

}
#endif // RINTERFACE_H
