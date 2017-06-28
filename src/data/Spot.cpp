#include "data/Spot.h"

Spot::Spot()
    : m_coordinates(0,0)
    , m_visible(false)
    , m_selected(false)
    , m_color(Qt::black)
    , m_name()
{
    updateName();
}

Spot::Spot(const float x, const float y)
    : m_coordinates(x,y)
    , m_visible(false)
    , m_selected(false)
    , m_color(Qt::white)
    , m_name()
{
    updateName();
}

Spot::Spot(const SpotType coordinates)
    : m_coordinates(coordinates)
    , m_visible(false)
    , m_selected(false)
    , m_color(Qt::white)
    , m_name()
{
    updateName();
}

Spot::Spot(const Spot &other)
{
    m_coordinates = other.m_coordinates;
    m_color = other.m_color;
    m_name = other.m_name;
    m_visible = other.m_visible;
    m_selected = other.m_selected;
}

Spot::~Spot()
{
}

Spot &Spot::operator=(const Spot &other)
{
    m_coordinates = other.m_coordinates;
    m_visible = other.m_visible;
    m_selected = other.m_selected;
    m_color = other.m_color;
    m_name = other.m_name;
    return (*this);
}

bool Spot::operator==(const Spot &other) const
{
    return (m_coordinates == other.m_coordinates
            && m_visible == other.m_visible
            && m_selected == other.m_selected
            && m_color == other.m_color
            && m_name == other.m_name);
}

Spot::SpotType Spot::coordinates() const
{
    return m_coordinates;
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

void Spot::coordinates(const float x, const float y)
{
    m_coordinates = SpotType(x,y);
    updateName();
}

void Spot::coordinates(const SpotType &coordinates)
{
    m_coordinates = coordinates;
    updateName();
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

void Spot::updateName()
{
    m_name = QString::number(m_coordinates.first) + "x" + QString::number(m_coordinates.second);
}
