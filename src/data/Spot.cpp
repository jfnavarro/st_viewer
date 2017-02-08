#include "data/Spot.h"

Spot::Spot()
    : m_count(0)
    , m_geneCount(0)
    , m_x(0)
    , m_y(0)
    , m_visible(false)
{
}

Spot::Spot(const Spot &other)
{
    m_count = other.m_count;
    m_geneCount = other.m_geneCount;
    m_x = other.m_x;
    m_y = other.m_y;
    m_visible = other.m_visible;
}

Spot::~Spot()
{
}

Spot &Spot::operator=(const Spot &other)
{
    m_count = other.m_count;
    m_geneCount = other.m_geneCount;
    m_x = other.m_x;
    m_y = other.m_y;
    m_visible = other.m_visible;
    return (*this);
}

bool Spot::operator==(const Spot &other) const
{
    return (m_count == other.m_count
            && m_geneCount == other.m_geneCount
            && m_x == other.m_x
            && m_y == other.m_y
            && m_visible == other.m_visible);
}

int Spot::count() const
{
    return m_count;
}

int Spot::geneCount() const
{
    return m_geneCount;
}

float Spot::x() const
{
    return m_x;
}

float Spot::y() const
{
    return m_y;
}

bool Spot::visible() const
{
    return m_visible;
}

void Spot::count(int count)
{
    m_count = count;
}

void Spot::geneCount(int geneCount)
{
    m_geneCount = geneCount;
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
