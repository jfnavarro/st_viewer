#include "data/Spot.h"

Spot::Spot()
    : m_x(0)
    , m_y(0)
    , m_visible(true)
    , m_selected(false)
    , m_color(Qt::white)
    , m_name()
{
}

Spot::Spot(const float x, const float y)
    : m_x(x)
    , m_y(y)
    , m_visible(true)
    , m_selected(false)
    , m_color(Qt::white)
    , m_name()
{

}

Spot::Spot(const Spot &other)
{
    m_x = other.m_x;
    m_y = other.m_y;
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
    m_x = other.m_x;
    m_y = other.m_y;
    m_visible = other.m_visible;
    m_selected = other.m_selected;
    m_color = other.m_color;
    m_name = other.m_name;
    return (*this);
}

bool Spot::operator==(const Spot &other) const
{
    return (m_x == other.m_x
            && m_y == other.m_y
            && m_visible == other.m_visible
            && m_selected == other.m_selected
            && m_color == other.m_color
            && m_name == other.m_name);
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

QColor Spot::color() const
{
    return m_color;
}

QString Spot::name()
{
    if (m_name.isEmpty() || m_name.isNull()) {
        m_name = QString::number(m_x) + "x" + QString::number(m_y);
    }
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

void Spot::selected(bool selected)
{
    m_selected = selected;
}

void Spot::color(QColor color)
{
    m_color = color;
}
