/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef COLORSCHEME_H
#define COLORSCHEME_H

#include "controller/data/DataProxy.h"

// Strategy pattern to allow features to be rendered with different colors.
class ColorScheme
{
public:
    explicit ColorScheme(int minHits, int maxHits);
    virtual ~ColorScheme() {};
    virtual QColor getColor(DataProxy::FeatureRef feature) const = 0;
    inline void setMin(int min) { m_minHits = min; }
    inline void setMax(int max) { m_maxHits = max; }
    inline int getMax() { return m_maxHits; }
    inline int getMin() { return m_minHits; }
protected:
    int m_minHits;
    int m_maxHits;
};

#endif // COLORSCHEME_H
