#ifndef SPOT_H
#define SPOT_H

// Data model class to store spot data
// Each spot correspond to a spot in the the array and it is
// defined by two int/float coordinates.
// Extra attributes for the spots are added in this data model
class Spot
{

public:

    Spot();
    explicit Spot(const Spot &other);
    ~Spot();

    Spot &operator=(const Spot &other);
    bool operator==(const Spot &other) const;

    // count represents the total sum of counts in the spot
    int count() const;
    // geneCount represents the total number of expressed genes in the spot
    int geneCount() const;
    // the spot's coordinates
    float x() const;
    float y() const;
    // true if the spot is visible
    bool visible() const;

    // Setters
    void count(int count);
    void geneCount(int geneCount);
    void x(float x);
    void y(float y);
    void visible(bool visible);

private:
    int m_count;
    int m_geneCount;
    float m_x;
    float m_y;
    bool m_visible;
};

#endif // SPOT_H
