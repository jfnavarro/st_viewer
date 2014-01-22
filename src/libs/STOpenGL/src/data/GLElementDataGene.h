/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLELEMENTDATAGENE_H
#define GLELEMENTDATAGENE_H

#include "GLElementData.h"

#include "utils/Utils.h"
#include <QArray>

namespace GL
{

// GLElementData is an extension of GLElementData to
// add some valuable data used to render
// IMPORTANT : this object needs to be render with shaders


class GLElementDataGene : public GLElementData
{
public:

    typedef QArray<GLuint> FeaturesType;
    typedef QArray<GLuint> ReferencesType;
    typedef QArray<GLuint> ValuesType;
    typedef QArray<GLuint> OptionsType;

    enum ClearFlags  {
        PointArray = 0x001u,
        ColorArray = 0x002u,
        IndexArray = 0x004u,
        TextureArray = 0x008u,
        OptionArray = 0x010u,
        FeatureCountArray = 0x012u,
        RefCountArray = 0x014u,
        ExpressionCountArray = 0x016u,
        RenderMode = 0x100u,
        // composite flags
        Arrays = (PointArray | ColorArray | IndexArray | TextureArray | OptionArray
        | FeatureCountArray | RefCountArray | ExpressionCountArray ),
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
    void resetFeatCount();
    void resetOptions();

    // number of genes in the feature
    void addFeatCount(const GLuint &featcount, GLindex *index = 0);

    // number of selected genes in the feature
    void addRefCount(const GLuint &refcount, GLindex *index = 0);

    // sum of the hits of the selected genes in the feature
    void addValue(const GLuint &value, GLindex *index = 0);

    // option (selected or not)
    void addOption(const GLuint &option, GLindex *index = 0);

    // number of genes in the feature
    void setFeatCount(const GLindex &index, const GLuint &featcount);

    // number of selected genes in the feature
    void setRefCount(const GLindex &index, const GLuint &refcount);

    // sum of the hits of the selected genes in the feature
    void setValue(const GLindex &index, const GLuint &value);

    // option (selected or not)
    void setOption(const GLindex &index, const GLuint &option);

    // getters for arrays by index
    const GLuint &getFeatCount(const GLindex &index) const;
    const GLuint &getRefCount(const GLindex &index) const;
    const GLuint &getValue(const GLindex &index) const;
    GLuint getOption(const GLindex &index) const;

    // getters for arrays
    const FeaturesType &features() const;
    const ReferencesType &references() const;
    const ValuesType &values() const;
    const OptionsType &options() const;

    // global variable (color mode)
    inline void setColorMode(const Globals::VisualMode &mode) { m_colorMode = mode; }

    // global variable (threshold mode)
    void setThresholdMode(const Globals::ThresholdMode &mode) { m_geneMode = mode; }

    // global variables (hit count limits)
    inline void setHitCount(int min, int max, int sum)
    {
        m_hitCountMin = min;
        m_hitCountMax = max;
        m_hitCountSum = sum;
    }

    // global variable intensity
    inline void setIntensity(const qreal &intensity) { m_intensity = intensity; }

    // thresholds limits
    inline void setLowerLimit(int limit) { m_geneLowerLimit = limit; }
    inline void setUpperLimit(int limit) { m_geneUpperLimit = limit; }
    
    // getters global varibles
    inline const Globals::VisualMode& getColorMode() const { return m_colorMode; }
    inline const Globals::ThresholdMode& getThresholdMode() const { return m_geneMode; }

    inline GLuint getHitCountMin() const { return m_hitCountMin; }
    inline GLuint getHitCountMax() const { return m_hitCountMax; }
    inline GLuint getHitCountSum() const { return m_hitCountSum; }

    inline GLuint getLowerLimit() const { return m_geneLowerLimit; }
    inline GLuint getUpperLimit() const { return m_geneUpperLimit; }

    inline GLfloat getIntensity() const { return m_intensity; }

    //check whether the arrays are empty or not
    inline bool isEmpty() const
    {
        return m_colors.empty() && m_indices.empty() && m_textures.empty()
                && m_points.empty() && m_options.empty() && m_features.empty()
                && m_references.empty() && m_values.empty();
    }

private:

    FeaturesType m_features;
    ReferencesType m_references;
    ValuesType m_values; //only used for global mode
    OptionsType m_options;

    Globals::VisualMode m_colorMode; //only used for global mode
    Globals::ThresholdMode m_geneMode; //only used for global mode

    // hit count values
    GLint m_hitCountMin; //only used for global mode
    GLint m_hitCountMax; //only used for global mode
    GLint m_hitCountSum; //only used for global mode

    // upper && lower thresholds
    GLint m_geneLowerLimit; //only used for global mode
    GLint m_geneUpperLimit; //only used for global mode

    GLfloat m_intensity;
};

} // namespace GL //


#endif // GLELEMENTDATAGENE_H
