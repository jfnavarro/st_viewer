/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef COLORSCHEME_H
#define COLORSCHEME_H

<<<<<<< HEAD
#include "core/data/DataProxy.h"
=======
#include "data/DataProxy.h"
>>>>>>> 39bfdff81cd1108639383d1d72dd0dc33bfb4925

// Strategy pattern to allow features to be rendered with different colors.
class ColorScheme
{

public:

    ColorScheme() {};
    virtual ~ColorScheme() {};

    virtual QColor getColor(const DataProxy::FeaturePtr feature, int m_min, int m_max) const = 0;
};

#endif // COLORSCHEME_H
