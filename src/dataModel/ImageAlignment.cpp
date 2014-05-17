#include "ImageAlignment.h"

ImageAlignment::ImageAlignment()
{

}

ImageAlignment::ImageAlignment(const ImageAlignment& other)
    : m_id(other.m_id),
      m_name(other.m_name),
      m_chipId(other.m_chipId),
      m_figureRed(other.m_figureRed),
      m_figureBlue(other.m_figureBlue),
      m_alignment(other.m_alignment)
{

}

ImageAlignment::~ImageAlignment()
{

}

ImageAlignment& ImageAlignment::operator=(const ImageAlignment& other)
{
    m_id = other.m_id;
    m_name = other.m_name;
    m_chipId = other.m_chipId;
    m_figureRed = other.m_figureRed;
    m_figureBlue = other.m_figureBlue;
    m_alignment = other.m_alignment;
    return (*this);
}

bool ImageAlignment::operator==(const ImageAlignment& other) const
{
    return(
            m_id == other.m_id &&
            m_name == other.m_name &&
            m_chipId == other.m_chipId &&
            m_figureBlue == other.m_figureBlue &&
            m_figureRed == other.m_figureRed &&
            m_alignment == other.m_alignment
        );
}
