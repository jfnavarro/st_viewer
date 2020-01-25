#ifndef SPOT_H
#define SPOT_H

#include <QPair>
#include <QColor>
#include <QVector3D>

// Data model class to store spot data
// Each spot correspond to a spot in the the array and it is
// defined by two int/double coordinates.
// Extra attributes for the spots are added in this data model
class Spot
{

public:

    typedef QVector3D SpotType;

    Spot();
    Spot(const QString name);
    explicit Spot(const Spot &other);
    ~Spot();

    Spot &operator=(const Spot &other);
    bool operator==(const Spot &other) const;

    // the spot's coordinates
    SpotType coordinates() const;
    // the spot's adjusted coordinates (only useful for plotting)
    SpotType adj_coordinates() const;
    // the spot's coordinates as a string
    QString name() const;
    // the spot's color
    QColor color() const;
    // true if the spot is visible
    bool visible() const;
    // true if the spot is selected
    bool selected() const;
    // meta info of the spot
    QString info() const;
    // the total number of transcripts for the spot in the dataset
    double totalCount() const;

    // Setters
    void coordinates(const SpotType &coordinates);
    void adj_coordinates(const SpotType &adj_coordinates);
    void name(const QString &name);
    void color(const QColor color);
    void visible(const bool visible);
    void selected(const bool selected);
    void info(const QString &info);
    void totalCount(const double totalCoun);

    // helper method to get coordinates (x,y,x) from a spot in string format
    static SpotType getCoordinates(const QString &spot);

private:
    SpotType m_coordinates;
    SpotType m_adj_coordinates;
    bool m_visible;
    bool m_selected;
    QColor m_color;
    QString m_name;
    QString m_info;
    double m_totalCount;
};

#endif // SPOT_H
