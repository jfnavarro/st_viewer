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
    typedef QPair<int, int> SpotType;
    typedef QSet<SpotType> UniqueSpotsType;
    typedef QHash<Feature::SpotType, int> spotTotalCounts;
    typedef QHash<QString, int> geneTotalCounts;

    Feature();
    explicit Feature(const Feature &other);
    Feature(const QString &gene, int x, int y, int count);
    ~Feature();

    Feature &operator=(const Feature &other);
    bool operator==(const Feature &other) const;

    const QString gene() const;
    // count represents the expression level
    int count() const;
    int x() const;
    int y() const;
    // the coordinates of the spot in the array
    SpotType spot() const;

    void gene(const QString &gene);
    void count(int count);
    void x(int x);
    void y(int y);

protected:
    // basic attributes
    QString m_gene;
    int m_count;
    int m_x;
    int m_y;
};

#endif // FEATURE_H
