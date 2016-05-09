#include "ImageAlignment.h"

#include <QDate>

ImageAlignment::ImageAlignment()
    : m_id()
    , m_name()
    , m_chipId()
    , m_figureRed()
    , m_figureBlue()
    , m_alignment()
    , m_created(QDate::currentDate().toString())
    , m_lastModified(QDate::currentDate().toString())
{
}

ImageAlignment::ImageAlignment(const ImageAlignment &other)
    : m_id(other.m_id)
    , m_name(other.m_name)
    , m_chipId(other.m_chipId)
    , m_figureRed(other.m_figureRed)
    , m_figureBlue(other.m_figureBlue)
    , m_alignment(other.m_alignment)
    , m_created(other.m_created)
    , m_lastModified(other.m_lastModified)
{
}

ImageAlignment::~ImageAlignment()
{
}

ImageAlignment &ImageAlignment::operator=(const ImageAlignment &other)
{
    m_id = other.m_id;
    m_name = other.m_name;
    m_chipId = other.m_chipId;
    m_figureRed = other.m_figureRed;
    m_figureBlue = other.m_figureBlue;
    m_alignment = other.m_alignment;
    m_created = other.m_created;
    m_lastModified = other.m_lastModified;
    return (*this);
}

bool ImageAlignment::operator==(const ImageAlignment &other) const
{
    return (m_id == other.m_id && m_name == other.m_name && m_chipId == other.m_chipId
            && m_figureBlue == other.m_figureBlue && m_figureRed == other.m_figureRed
            && m_alignment == other.m_alignment && m_created == other.m_created
            && m_lastModified == other.m_lastModified);
}

const QString ImageAlignment::id()
{
    return m_id;
}

const QString ImageAlignment::name() const
{
    return m_name;
}

const QString ImageAlignment::chipId() const
{
    return m_chipId;
}

const QString ImageAlignment::figureRed() const
{
    return m_figureRed;
}

const QString ImageAlignment::figureBlue() const
{
    return m_figureBlue;
}

const QTransform ImageAlignment::alignment() const
{
    return m_alignment;
}

const QString ImageAlignment::created() const
{
    return m_created;
}

const QString ImageAlignment::lastModified() const
{
    return m_lastModified;
}

void ImageAlignment::id(const QString &id)
{
    m_id = id;
}

void ImageAlignment::name(const QString &name)
{
    m_name = name;
}

void ImageAlignment::chipId(const QString &chipId)
{
    m_chipId = chipId;
}

void ImageAlignment::figureRed(const QString &figureRed)
{
    m_figureRed = figureRed;
}

void ImageAlignment::figureBlue(const QString &figureBlue)
{
    m_figureBlue = figureBlue;
}

void ImageAlignment::alignment(const QTransform &alignment)
{
    m_alignment = alignment;
}

void ImageAlignment::created(const QString &created)
{
    m_created = created;
}

void ImageAlignment::lastModified(const QString &lastModified)
{
    m_lastModified = lastModified;
}
