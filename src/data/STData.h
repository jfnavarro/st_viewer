#ifndef STDATA_H
#define STDATA_H

#include <QSharedPointer>
#include <QList>
#include "data/Gene.h"
#include "data/Spot.h"
#include <armadillo>

using namespace arma;

class STData
{

public:

    enum Normalization { RAW = 0, DESeq = 1, TPM = 2, REL = 3, SCRAN };

    typedef QPair<float,float> SpotType;
    typedef QString GeneType;
    typedef QSharedPointer<Spot> SpotObjectType;
    typedef QSharedPointer<Gene> GeneObjectType;
    typedef QList<SpotObjectType> SpotListType;
    typedef QList<GeneObjectType> GeneListType;

    STData();
    explicit STData(const STData &other);
    ~STData();

    STData &operator=(const STData &other);
    bool operator==(const STData &other) const;

    void read(const QString &filename);
    void save(const QString &filename) const;

    void normalize(Normalization normalization);

    std::vector<float> count(const GeneType &gene) const;
    std::vector<float> count(const SpotType &spot) const;
    float count(const GeneType &gene, const SpotType &spot) const;

    std::vector<float> count(const GeneObjectType &gene) const;
    std::vector<float> count(const SpotObjectType &spot) const;
    float count(const GeneObjectType &gene, const SpotObjectType &spot) const;

    Mat<float> slice_matrix_counts() const;
    Mat<float> matrix_counts() const;

    std::vector<float> spots_counts();
    std::vector<float> genes_counts();

    QVector<QColor> spots_colors() const;

    GeneType gene_at(size_t index) const;
    SpotType spot_at(size_t index) const;
    GeneObjectType gene_object_at(size_t index) const;
    SpotObjectType spot_object_at(size_t index) const;

    size_t number_spots() const;
    size_t number_genes() const;

private:

    Mat<float> m_counts_matrix;
    Mat<float> m_counts_norm_matrix;
    Normalization m_normalization;
    SpotListType m_spots;
    GeneListType m_genes;
    QVector<GeneType> m_matrix_genes;
    QVector<SpotType> m_matrix_spots;
    float m_spot_count_threshold;
    float m_gene_count_threshold;
    float m_spot_gene_count_threshold;
};

#endif // STDATA_H
