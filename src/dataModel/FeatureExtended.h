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
    explicit FeatureExtended(const Feature& other);
    explicit FeatureExtended(const FeatureExtended &other);
    virtual ~FeatureExtended();

    FeatureExtended& operator=(const Feature& other);
    FeatureExtended& operator=(const FeatureExtended& other);
    bool operator==(const FeatureExtended& other) const;

    const QColor& color() const { return m_color; }
    void color(const QColor& color) { m_color = color; }

    bool selected() const {return m_selected;}
    void selected(bool selected) {m_selected = selected;}

private:
    
    QColor m_color;
    bool m_selected;
};

#endif // FEATUREEXTENDED_H
