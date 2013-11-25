/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef HITCOUNT_H
#define HITCOUNT_H

#include "utils/Utils.h"

// Data model class to store hit count data.
class HitCount
{

public:

    HitCount();
    HitCount(const HitCount& other);
    ~HitCount();

    HitCount& operator= (const HitCount& other);
    bool operator== (const HitCount& other) const;

    int min() const
    {
        return m_min;
    }
    int max() const
    {
        return m_max > Globals::limit_hit_count ? Globals::limit_hit_count : m_max;
    }
    int sum() const
    {
        return m_sum;
    }

    void min(int min)
    {
        m_min = min;
    }
    void max(int max)
    {
        m_max = max;
    }
    void sum(int sum)
    {
        m_sum = sum;
    }

private:

    int m_min;
    int m_max;
    int m_sum;

};

#endif // HITCOUNT_H //
