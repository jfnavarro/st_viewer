/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <climits>

#include "HitCount.h"

HitCount::HitCount()
    : m_min(0), m_max(INT_MAX), m_sum(-1)
{

}

HitCount::HitCount(const HitCount& other)
{
    m_min = other.m_min;
    m_max = other.m_max;
    m_sum = other.m_sum;
}

HitCount::~HitCount()
{

}

HitCount& HitCount::operator=(const HitCount& other)
{
    m_min = other.m_min;
    m_max = other.m_max;
    m_sum = other.m_sum;
    return (*this);
}

bool HitCount::operator==(const HitCount& other) const
{
    return (m_min == other.m_min &&
            m_max == other.m_max &&
            m_sum == other.m_sum
        );
}
