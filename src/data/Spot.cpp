#include "data/Spot.h"

Spot::Spot()
    : m_x(0)
    , m_y(0)
    , m_visible(false)
{
}

Spot::Spot(const float x, const float y)
    : m_x(x)
    , m_y(y)
    , m_visible(false)
{

}

Spot::Spot(const Spot &other)
{
    m_x = other.m_x;
    m_y = other.m_y;
    m_visible = other.m_visible;
}

Spot::~Spot()
{
}

Spot &Spot::operator=(const Spot &other)
{
    m_x = other.m_x;
    m_y = other.m_y;
    m_visible = other.m_visible;
    return (*this);
}

bool Spot::operator==(const Spot &other) const
{
    return (m_x == other.m_x
            && m_y == other.m_y
            && m_visible == other.m_visible);
}

float Spot::x() const
{
    return m_x;
}

float Spot::y() const
{
    return m_y;
}

QPair<float, float> Spot::coordinates() const
{
    return QPair<float,float>(m_x, m_y);
}

bool Spot::visible() const
{
    return m_visible;
}

void Spot::x(float x)
{
    m_x = x;
}

void Spot::y(float y)
{
    m_y = y;
}

void Spot::visible(bool visible)
{
    m_visible = visible;
}
