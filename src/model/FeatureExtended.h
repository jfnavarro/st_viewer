/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef FEATUREEXTENDED_H
#define FEATUREEXTENDED_H

#include <QColor>

#include "Feature.h"

// Extended data model class to store colour annotated feature data.
class FeatureExtended : public Feature
{
public:
    FeatureExtended();
    FeatureExtended(const Feature& other);
    FeatureExtended(const FeatureExtended &other);
    virtual ~FeatureExtended();

    FeatureExtended& operator=(const Feature& other);
    FeatureExtended& operator=(const FeatureExtended& other);

    bool operator==(const FeatureExtended& other) const;

    inline const QColor& color() const { return m_color; }
    inline void color(const QColor& color) { m_color = color; }

private:    
    QColor m_color;
};

#endif // FEATUREEXTENDED_H
