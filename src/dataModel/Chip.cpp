#include "Chip.h"

#include <QDate>

Chip::Chip()
    : m_id()
    , m_spots(0)
    , m_name()
    , m_x1(0)
    , m_x2(0)
    , m_x1Total(0)
    , m_x2Total(0)
    , m_x1Border(0)
    , m_x2Border(0)
    , m_y1(0)
    , m_y2(0)
    , m_y1Total(0)
    , m_y2Total(0)
    , m_y1Border(0)
    , m_y2Border(0)
    , m_created(QDate::currentDate().toString())
    , m_lastMofidied(QDate::currentDate().toString())
{
}

Chip::Chip(const Chip &other)
{
    m_id = other.m_id;
    m_spots = other.m_spots;
    m_name = other.m_name;
    m_x1 = other.m_x1;
    m_x2 = other.m_x2;
    m_y1 = other.m_y1;
    m_y2 = other.m_y2;
    m_x1Border = other.m_x1Border;
    m_x2Border = other.m_x2Border;
    m_y1Border = other.m_y1Border;
    m_y2Border = other.m_y2Border;
    m_x1Total = other.m_x1Total;
    m_x2Total = other.m_x2Total;
    m_y1Total = other.m_y1Total;
    m_y2Total = other.m_y2Total;
    m_created = other.m_created;
    m_lastMofidied = other.m_lastMofidied;
}

Chip::~Chip()
{
}

Chip &Chip::operator=(const Chip &other)
{
    m_id = other.m_id;
    m_spots = other.m_spots;
    m_name = other.m_name;
    m_x1 = other.m_x1;
    m_x2 = other.m_x2;
    m_y1 = other.m_y1;
    m_y2 = other.m_y2;
    m_x1Border = other.m_x1Border;
    m_x2Border = other.m_x2Border;
    m_y1Border = other.m_y1Border;
    m_y2Border = other.m_y2Border;
    m_x1Total = other.m_x1Total;
    m_x2Total = other.m_x2Total;
    m_y1Total = other.m_y1Total;
    m_y2Total = other.m_y2Total;
    m_created = other.m_created;
    m_lastMofidied = other.m_lastMofidied;
    return (*this);
}

bool Chip::operator==(const Chip &other) const
{
    return (m_id == other.m_id && m_spots == other.m_spots && m_name == other.m_name
            && m_x1 == other.m_x1 && m_x2 == other.m_x2 && m_y1 == other.m_y1 && m_y2 == other.m_y2
            && m_x1Border == other.m_x1Border && m_x2Border == other.m_x2Border
            && m_y1Border == other.m_y1Border && m_y2Border == other.m_y2Border
            && m_x1Total == other.m_x1Total && m_x2Total == other.m_x2Total
            && m_y1Total == other.m_y1Total && m_y2Total == other.m_y2Total
            && m_created == other.m_created && m_lastMofidied == other.m_lastMofidied);
}

const QString Chip::id() const
{
    return m_id;
}

unsigned Chip::spots() const
{
    return m_spots;
}

const QString Chip::name() const
{
    return m_name;
}

unsigned Chip::x1() const
{
    return m_x1;
}

unsigned Chip::x2() const
{
    return m_x2;
}

unsigned Chip::x1Total() const
{
    return m_x1Total;
}

unsigned Chip::x2Total() const
{
    return m_x2Total;
}

unsigned Chip::x1Border() const
{
    return m_x1Border;
}

unsigned Chip::x2Border() const
{
    return m_x2Border;
}

unsigned Chip::y1() const
{
    return m_y1;
}

unsigned Chip::y2() const
{
    return m_y2;
}

unsigned Chip::y1Total() const
{
    return m_y1Total;
}

unsigned Chip::y2Total() const
{
    return m_y2Total;
}

unsigned Chip::y1Border() const
{
    return m_y1Border;
}

unsigned Chip::y2Border() const
{
    return m_y2Border;
}

const QString Chip::created() const
{
    return m_created;
}

const QString Chip::lastModified() const
{
    return m_lastMofidied;
}

void Chip::id(const QString &id)
{
    m_id = id;
}

void Chip::spots(unsigned spots)
{
    m_spots = spots;
}

void Chip::name(const QString &name)
{
    m_name = name;
}

void Chip::x1(unsigned x1)
{
    m_x1 = x1;
}

void Chip::x2(unsigned x2)
{
    m_x2 = x2;
}

void Chip::x1Total(unsigned x1Total)
{
    m_x1Total = x1Total;
}

void Chip::x2Total(unsigned x2Total)
{
    m_x2Total = x2Total;
}

void Chip::x1Border(unsigned x1Border)
{
    m_x1Border = x1Border;
}

void Chip::x2Border(unsigned x2Border)
{
    m_x2Border = x2Border;
}

void Chip::y1(unsigned y1)
{
    m_y1 = y1;
}

void Chip::y2(unsigned y2)
{
    m_y2 = y2;
}

void Chip::y1Total(unsigned y1Total)
{
    m_y1Total = y1Total;
}

void Chip::y2Total(unsigned y2Total)
{
    m_y2Total = y2Total;
}

void Chip::y1Border(unsigned y1Border)
{
    m_y1Border = y1Border;
}

void Chip::y2Border(unsigned y2Border)
{
    m_y2Border = y2Border;
}

void Chip::created(const QString &created)
{
    m_created = created;
}

void Chip::lastModified(const QString &lastModified)
{
    m_lastMofidied = lastModified;
}
