/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLELEMENTDATAGENE_H
#define GLELEMENTDATAGENE_H

#include "GLElementData.h"
#include "utils/Utils.h"

namespace GL
{

// GLElementData is an extension of GLElementData to
// add some valuable data used to render
// IMPORTANT : this object needs to be render with shaders
class GLElementDataGene : public GLElementData
{
public:

    enum ClearFlags {
        PointArray = 0x001u,
        ColorArray = 0x002u,
        IndexArray = 0x004u,
        TextureArray = 0x008u,
        OptionArray = 0x010u,
        FeaturesArray = 0x012u,
        RefCountArray = 0x014u,
        ValueArray = 0x016u,
        RenderMode = 0x100u,
        // composite flags
        Arrays = (PointArray | ColorArray | IndexArray | TextureArray | OptionArray
        | FeaturesArray | RefCountArray | ValueArray ),
        Modes = (RenderMode),
        All = (Arrays | Modes)
    };

    GLElementDataGene();
    virtual ~GLElementDataGene();

    //clear arrays
    void clear(GLflag flags = GLElementDataGene::All);

    // some helpful reset functions (reset to 0)
    void resetRefCount();
    void resetValCount();

    // some helpful functions
    GLuint getMaxValue() const;
    GLuint getMinValue() const;

    // number of genes in the feature
    GLElementDataGene &addFeatCount(const GLuint &featcount, GLindex *index = 0);

    // number of selected genes in the feature
    GLElementDataGene &addRefCount(const GLuint &refcount, GLindex *index = 0);

    // sum of the hits of the selected genes in the feature
    GLElementDataGene &addValue(const GLuint &value, GLindex *index = 0);

    // number of genes in the feature
    void setFeatCount(const GLindex &index, const GLuint &featcount);

    // number of selected genes in the feature
    void setRefCount(const GLindex &index, const GLuint &refcount);

    // sum of the hits of the selected genes in the feature
    void setValue(const GLindex &index, const GLuint &value);

    // getters for arrays by index
    const GLuint &getFeatCount(const GLindex &index) const;
    const GLuint &getRefCount(const GLindex &index) const;
    const GLuint &getValue(const GLindex &index) const;

    // getters for arrays
    const GLarray<GLuint> features() const;
    const GLarray<GLuint> references() const;
    const GLarray<GLuint> values() const;

    // global variable (color mode)
    inline void setColorMode(const Globals::VisualMode &mode) { m_colorMode = mode; }

    // global variable (threshold mode)
    void setThresholdMode(const Globals::ThresholdMode &mode) { m_geneMode = mode; }

    // global variables (hit count limits)
    inline void setHitCount(int min, int max, int sum) { m_min = min; m_max = max; m_sum = sum; }

    // global variable intensity
    inline void setIntensity(const GLfloat &intensity) { m_intensity = intensity; }

    // thresholds limits
    inline void setLowerLimit(int geneLimit) { m_min = geneLimit; }
    inline void setUpperLimit(int geneLimit) { m_max = geneLimit; }
    inline void setTotalSum(int sum) { m_sum = sum; }
    
    // getters global varibles
    inline const Globals::VisualMode& getColorMode() const { return m_colorMode; }
    inline const Globals::ThresholdMode& getThresholdMode() const { return m_geneMode; }

    inline int getMin() const { return m_min; }
    inline int getMax() const { return m_max; }
    inline int getSum() const { return m_sum; }

    inline GLfloat getIntensity() const { return m_intensity; }

    //check whether the arrays are empty or not
    inline bool isEmpty() const
    {
        return m_colors.empty() && m_indices.empty() && m_textures.empty()
                && m_points.empty() && m_options.empty() && m_features.empty()
                && m_references.empty() && m_values.empty();
    }

private:

    typedef QVector<GLuint> GLFeatures;
    typedef QVector<GLuint> GLReferences;
    typedef QVector<GLuint> GLValues;

    GLFeatures m_features;
    GLReferences m_references;
    GLValues m_values;
    Globals::VisualMode m_colorMode;
    Globals::ThresholdMode m_geneMode;

    int m_min;
    int m_max;
    int m_sum;

    // upper && lower thresholds
    int m_geneLowerLimit;
    int m_geneUpperLimit;

    GLfloat m_intensity;
};

} // namespace GL //


#endif // GLELEMENTDATAGENE_H
