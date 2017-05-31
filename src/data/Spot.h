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
    Spot(const float x, const float y);
    Spot(const SpotType coordinates);
    explicit Spot(const Spot &other);
    ~Spot();

    Spot &operator=(const Spot &other);
    bool operator==(const Spot &other) const;

    // the spot's coordinates
    SpotType coordinates() const;
    // name returns the X and Y coordiantes as a string
    QString name() const;
    // the spot's color
    QColor color() const;
    // true if the spot is visible
    bool visible() const;
    // true if the spot is selected
    bool selected() const;

    // Setters
    void coordinates(const float x, const float y);
    void coordinates(const SpotType &coordinates);
    void color(const QColor color);
    void visible(const bool visible);
    void selected(const bool selected);
    void name(const QString &name);

private:
    void updateName();
    SpotType m_coordinates;
    bool m_visible;
    bool m_selected;
    QColor m_color;
    QString m_name;
};

#endif // SPOT_H
