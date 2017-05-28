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

    Spot();
    Spot(const float x, const float y);
    explicit Spot(const Spot &other);
    ~Spot();

    Spot &operator=(const Spot &other);
    bool operator==(const Spot &other) const;

    // the spot's coordinates
    float x() const;
    float y() const;
    QPair<float, float> coordinates() const;
    // name returns the X and Y coordiantes as a string
    QString name();
    QColor color() const;
    // true if the spot is visible
    bool visible() const;
    // true if the spot is selected
    bool selected() const;

    // Setters
    void x(float x);
    void y(float y);
    void color(QColor color);
    void visible(bool visible);
    void selected(bool selected);

private:
    float m_x;
    float m_y;
    bool m_visible;
    bool m_selected;
    QColor m_color;
    QString m_name;
};

#endif // SPOT_H
