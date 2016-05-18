#ifndef FEATURE_H
#define FEATURE_H

#include <QString>
#include <QHash>
#include <QSet>

// Data model class to store feature data
// A feature corresponds to a tuple (barcode or spot in the array)
// and a gene. In each barcode/spot there can be up to 20k genes.
// The coordinates x,y refers to chip coordinates but the chip object
// contains an affine matrix that converts chip coordinates to image pixel
// coordinates
// which is what we eventually visualize in the cell view.
class Feature
{

public:
    // Only for pairs of std::hash-able types for simplicity.
    // You can of course template this struct to allow other hash functions
    typedef QPair<unsigned, unsigned> SpotType;
    typedef QSet<SpotType> UniqueSpotsType;
    typedef QHash<Feature::SpotType, unsigned> spotTotalCounts;
    typedef QHash<QString, unsigned> geneTotalCounts;

    Feature();
    explicit Feature(const Feature &other);
    Feature(const QString &gene, unsigned x, unsigned y, unsigned count);
    ~Feature();

    Feature &operator=(const Feature &other);
    bool operator==(const Feature &other) const;

    const QString gene() const;
    // count represents the expression level
    unsigned count() const;
    unsigned x() const;
    unsigned y() const;
    // the coordinates of the spot in the array
    SpotType spot() const;

    void gene(const QString &gene);
    void count(unsigned count);
    void x(unsigned x);
    void y(unsigned y);

protected:
    // basic attributes
    QString m_gene;
    unsigned m_count;
    unsigned m_x;
    unsigned m_y;
};

#endif // FEATURE_H
