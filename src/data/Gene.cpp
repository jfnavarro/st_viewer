#include "Gene.h"

Gene::Gene()
    : m_name()
    , m_color(Qt::red)
    , m_visible(false)
    , m_cutoff(1)
{
}

Gene::Gene(const Gene &other)
    : m_name(other.m_name)
    , m_color(other.m_color)
    , m_visible(other.m_visible)
    , m_cutoff(other.m_cutoff)
{
}

Gene::Gene(const QString &name, bool visible, const QColor &color, const int cutoff)
    : m_name(name)
    , m_color(color)
    , m_visible(visible)
    , m_cutoff(cutoff)
{
}

Gene::~Gene()
{
}

Gene &Gene::operator=(const Gene &other)
{
    m_name = other.m_name;
    m_visible = other.m_visible;
    m_color = other.m_color;
    m_cutoff = other.m_cutoff;
    return (*this);
}

bool Gene::operator==(const Gene &other) const
{
    return (m_visible == other.m_visible && m_name == other.m_name && m_color == other.m_color
            && m_cutoff == other.m_cutoff);
}

const QString Gene::name() const
{
    return m_name;
}

bool Gene::visible() const
{
    return m_visible;
}

const QColor Gene::color() const
{
    return m_color;
}

int Gene::cut_off() const
{
    return m_cutoff;
}

void Gene::name(const QString &name)
{
    m_name = name;
}

void Gene::visible(bool visible)
{
    m_visible = visible;
}

void Gene::color(const QColor &color)
{
    m_color = color;
}

void Gene::cut_off(const int cutoff)
{
    m_cutoff = cutoff;
}

bool Gene::isAmbiguous() const
{
    return m_name.startsWith("ambiguous", Qt::CaseSensitive);
}
