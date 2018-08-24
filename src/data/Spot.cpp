#include "data/Spot.h"

Spot::Spot()
    : m_coordinates(0,0)
    , m_visible(false)
    , m_selected(false)
    , m_color(Qt::black)
    , m_name()
    , m_totalCount(0)
{

}

Spot::Spot(const QString name)
    : m_coordinates(0,0)
    , m_adj_coordinates(0,0)
    , m_visible(false)
    , m_selected(false)
    , m_color(Qt::white)
    , m_name(name)
    , m_totalCount(0)
{

    m_coordinates = getCoordinates(name);
    m_adj_coordinates = m_coordinates;
}

Spot::Spot(const Spot &other)
{
    m_coordinates = other.m_coordinates;
    m_adj_coordinates = other.m_adj_coordinates;
    m_color = other.m_color;
    m_name = other.m_name;
    m_visible = other.m_visible;
    m_selected = other.m_selected;
    m_totalCount = other.m_totalCount;
}

Spot::~Spot()
{
}

Spot &Spot::operator=(const Spot &other)
{
    m_coordinates = other.m_coordinates;
    m_adj_coordinates = other.m_adj_coordinates;
    m_visible = other.m_visible;
    m_selected = other.m_selected;
    m_color = other.m_color;
    m_name = other.m_name;
    m_totalCount = other.m_totalCount;
    return (*this);
}

bool Spot::operator==(const Spot &other) const
{
    return (m_coordinates == other.m_coordinates
            && m_adj_coordinates == other.m_adj_coordinates
            && m_visible == other.m_visible
            && m_selected == other.m_selected
            && m_color == other.m_color
            && m_name == other.m_name
            && m_totalCount == other.m_totalCount);
}

Spot::SpotType Spot::coordinates() const
{
    return m_coordinates;
}

Spot::SpotType Spot::adj_coordinates() const
{
    return m_adj_coordinates;
}

QColor Spot::color() const
{
    return m_color;
}

QString Spot::name() const
{
    return m_name;
}

bool Spot::visible() const
{
    return m_visible;
}

bool Spot::selected() const
{
    return m_selected;
}

double Spot::totalCount() const
{
    return m_totalCount;
}

void Spot::coordinates(const SpotType &coordinates)
{
    m_coordinates = coordinates;
}

void Spot::adj_coordinates(const SpotType &adj_coordinates)
{
    m_adj_coordinates = adj_coordinates;
}

void Spot::visible(const bool visible)
{
    m_visible = visible;
}

void Spot::selected(const bool selected)
{
    m_selected = selected;
}

void Spot::color(const QColor color)
{
    m_color = color;
}

void Spot::totalCount(const double totalCount)
{
    m_totalCount = totalCount;
}


Spot::SpotType Spot::getCoordinates(const QString &spot)
{
    const QStringList items = spot.trimmed().split("x");
    Q_ASSERT(items.size() == 2 || items.size() == 3);
    const double x = items.at(0).toDouble();
    const double y = items.at(1).toDouble();
    const double z = items.size() == 3 ? items.at(2).toDouble() : 0;
    return SpotType(x,y,z);
}

QString Spot::getSpot(const Spot::SpotType &spot)
{ 
    return QString::number(spot.x) + "x" + QString::number(spot.y);
}

QString Spot::getSpot3D(const Spot::SpotType &spot)
{
    return QString::number(spot.x) + "x" + QString::number(spot.y) + "x" + QString::number(spot.z);
}
