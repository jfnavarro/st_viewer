#ifndef SPOT_H
#define SPOT_H

#include <QPair>
#include <QColor>

// Data model class to store spot data
// Each spot correspond to a spot in the the array and it is
// defined by two int/float coordinates.
// Extra attributes for the spots are added in this data model
class Spot
{

public:
    typedef QPair<float,float> SpotType;

    Spot();
    Spot(const QString name);
    explicit Spot(const Spot &other);
    ~Spot();

    Spot &operator=(const Spot &other);
    bool operator==(const Spot &other) const;

    // the spot's coordinates
    SpotType coordinates() const;
    // the spot's coordinates as a string
    QString name() const;
    // the spot's color
    QColor color() const;
    // true if the spot is visible
    bool visible() const;
    // true if the spot is selected
    bool selected() const;
    // the total number of transcripts for the spot in the dataset
    float totalCount() const;

    // Setters
    void color(const QColor color);
    void visible(const bool visible);
    void selected(const bool selected);
    void name(const QString &name);
    void totalCount(const float totalCoun);

    // helper method to get coordinates (x,y) from a spot
    static SpotType getCoordinates(const QString &spot);
    // helper method to string representation (XxY) of a spot
    static QString getSpot(const SpotType &spot);

private:
    SpotType m_coordinates;
    bool m_visible;
    bool m_selected;
    QColor m_color;
    QString m_name;
    float m_totalCount;
};

#endif // SPOT_H
