#include "Gene.h"

Gene::Gene()
    : m_name()
    , m_color(Qt::red)
    , m_visible(false)
    , m_cutoff(0)
    , m_totalCount(0)
{
}

Gene::Gene(const QString &name)
    : m_name(name)
    , m_color(Qt::red)
    , m_visible(false)
    , m_cutoff(0)
    , m_totalCount(0)
{
}

Gene::Gene(const Gene &other)
    : m_name(other.m_name)
    , m_color(other.m_color)
    , m_visible(other.m_visible)
    , m_cutoff(other.m_cutoff)
    , m_totalCount(other.m_totalCount)
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
    m_totalCount = other.m_totalCount;
    return (*this);
}

bool Gene::operator==(const Gene &other) const
{
    return (m_visible == other.m_visible
            && m_name == other.m_name
            && m_color == other.m_color
            && m_cutoff == other.m_cutoff
            && m_totalCount == other.m_totalCount);
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

double Gene::cut_off() const
{
    return m_cutoff;
}

double Gene::totalCount() const
{
    return m_totalCount;
}

void Gene::name(const QString &name)
{
    m_name = name;
}

void Gene::visible(const bool visible)
{
    m_visible = visible;
}

void Gene::color(const QColor &color)
{
    m_color = color;
}

void Gene::cut_off(const double cutoff)
{
    m_cutoff = cutoff;
}

void Gene::totalCount(const double totalCount)
{
    m_totalCount = totalCount;
}
