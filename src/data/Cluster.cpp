#include "Cluster.h"

Cluster::Cluster()
{

}

Cluster::~Cluster()
{
}

Cluster &Cluster::operator=(const Cluster &other)
{
    m_spots = other.m_spots;
    m_name = other.m_name;
    m_color = other.m_color;
    m_visible = other.m_visible;
    return (*this);
}

bool Cluster::operator==(const Cluster &other) const
{
    return (m_spots == other.m_spots
            && m_name == other.m_name
            && m_color == other.m_color
            && m_visible == other.m_visible);
}

const Cluster::ClusterType &Cluster::spots() const
{
    return m_spots;
}

const QString Cluster::name() const
{
    return m_name;
}

const QColor Cluster::color() const
{
    return m_color;
}

bool Cluster::visible() const
{
    return m_visible;
}

int Cluster::size() const
{
    return m_spots.size();
}

void Cluster::spots(const ClusterType &spots)
{
    m_spots = spots;
}

void Cluster::name(const QString name)
{
    m_name = name;
}

void Cluster::color(const QColor color)
{
    m_color = color;
}

void Cluster::visible(const bool visible)
{
    m_visible = visible;
}

