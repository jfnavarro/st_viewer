#ifndef GLELEMENTDATAGENE_H
#define GLELEMENTDATAGENE_H

#include "GLElementData.h"
#include "utils/Utils.h"

namespace GL
{

// GLElementData is an extension of GLElementData to
// add some valuable data used to render (shaders)
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

    inline GLElementDataGene();
    inline virtual ~GLElementDataGene();

    //clear arrays
    inline void clear(GLflag flags = GLElementDataGene::All);

    // some helpful reset functions (reset to 0)
    inline void resetRefCount();
    inline void resetValCount();

    // number of genes in the feature
    inline GLElementDataGene &addFeatCount(const GLuint &featcount, GLindex *index = 0);
    // number of selected genes in the feature
    inline GLElementDataGene &addRefCount(const GLuint &refcount, GLindex *index = 0);
    // sum of the hits of the selected genes in the feature
    inline GLElementDataGene &addValue(const GLuint &value, GLindex *index = 0);

    // number of genes in the feature
    inline void setFeatCount(const GLindex index, const GLuint &featcount);
    // number of selected genes in the feature
    inline void setRefCount(const GLindex index, const GLuint &refcount);
    // sum of the hits of the selected genes in the feature
    inline void setValue(const GLindex index, const GLuint &value);

    // getters for arrays by index
    inline GLuint getFeatCount(const GLindex index) const;
    inline GLuint getRefCount(const GLindex index) const;
    inline GLuint getValue(const GLindex index) const;

    inline GLuint getMaxValue() const;
    inline GLuint getMinValue() const;

    // getters for arrays
    inline const GLarray<GLuint> features() const;
    inline const GLarray<GLuint> references() const;
    inline const GLarray<GLuint> values() const;

    // global variable (color mode)
    inline void setColorMode(const Globals::VisualMode &mode) { m_colorMode = mode; }
    // global variable (threshold mode)
    inline void setThresholdMode(const Globals::ThresholdMode &mode) { m_geneMode = mode; }
    // global variables (hit count limits)
    inline void setHitCount(int min, int max, int sum) { m_min = min; m_max = max; m_sum = sum; }
    // global variable intensity
    inline void setIntensity(const GLfloat &intensity) { m_intensity = intensity; }
    // thresholds limits
    inline void setLowerLimit(int geneLimit) { m_geneLowerLimit = geneLimit; }
    inline void setUpperLimit(int geneLimit) { m_geneUpperLimit = geneLimit; }

    // getters global varibles
    inline const Globals::VisualMode& getColorMode() const { return m_colorMode; }
    inline const Globals::ThresholdMode& getThresholdMode() const { return m_geneMode; }
    inline int getMin() const { return m_min; }
    inline int getMax() const { return m_max; }
    inline int getSum() const { return m_sum; }
    inline GLfloat getIntensity() const { return m_intensity; }
    inline int getLowerLimit() const { return m_geneLowerLimit; }
    inline int getUpperLimit() const { return m_geneUpperLimit; }

    //check whether the arrays are empty or not
    inline bool isEmpty() const
    {
        return m_colors.empty() && m_indices.empty() && m_textures.empty()
                && m_points.empty() && m_options.empty() && m_features.empty()
                && m_references.empty() && m_values.empty();
    }

private:

    typedef std::vector<GLuint> GLFeatures;
    typedef std::vector<GLuint> GLReferences;
    typedef std::vector<GLuint> GLValues;

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

/****************************************** DEFINITION ******************************************/

namespace GL
{


GLElementDataGene::GLElementDataGene():
    GLElementData(), m_features(), m_references(), m_values()
{

}

GLElementDataGene::~GLElementDataGene()
{

}

void GLElementDataGene::clear(GLflag flags)
{

    if (flags & GLElementDataGene::FeaturesArray) {
        m_features.resize(0);
    }
    if (flags & GLElementDataGene::RefCountArray) {
        m_references.resize(0);
    }
    if (flags & GLElementDataGene::ValueArray) {
        m_values.resize(0);
    }

    m_colorMode = Globals::NormalMode;
    m_geneMode = Globals::IndividualGeneMode;
    m_min = Globals::gene_lower_limit;
    m_max = Globals::gene_upper_limit;
    m_sum = Globals::gene_upper_limit;
    m_intensity = Globals::gene_intensity;
    m_geneLowerLimit = Globals::gene_lower_limit;
    m_geneUpperLimit = Globals::gene_upper_limit;
    //call base class
    GLElementData::clear();
}

void GLElementDataGene::resetRefCount()
{
    for (GLindex index = 0; index < (GLindex) m_references.size(); index++) {
        GLuint *data = reinterpret_cast<GLuint*>(&m_references[index]);
        (*data) = 0u;
    }
}

void GLElementDataGene::resetValCount()
{
    for (GLindex index = 0; index < (GLindex) m_values.size(); index++) {
        GLuint *data = reinterpret_cast<GLuint*>(&m_values[index]);
        (*data) = 0u;
    }
}

GLuint GLElementDataGene::getMaxValue() const
{
    GLuint maxvalue = 0;
    for (GLindex index = 0; index < (GLindex) m_values.size(); index++) {
        const GLuint *data = reinterpret_cast<const GLuint*>(&m_values[index]);
        maxvalue = max(maxvalue,(*data));
    }
    return maxvalue;
}

GLuint GLElementDataGene::getMinValue() const
{
    GLuint minvalue = 100000;
    for (GLindex index = 0; index < (GLindex) m_values.size(); index++) {
        const GLuint *data = reinterpret_cast<const GLuint*>(&m_values[index]);
        minvalue = min(minvalue,(*data));
    }
    return minvalue;
}

inline GLElementDataGene &GLElementDataGene::addFeatCount(const GLuint &featcount, GLindex *index)
{
    // return new index if pointer provided
    if (index != 0) {
        (*index) = (GLindex) m_features.size();
    }
    m_features.push_back(featcount);
    return (*this);
}

inline GLElementDataGene &GLElementDataGene::addRefCount(const GLuint &refcount, GLindex *index)
{
    // return new index if pointer provided
    if (index != 0) {
        (*index) = (GLindex) m_references.size();
    }
    m_references.push_back(refcount);
    return (*this);
}

inline GLElementDataGene &GLElementDataGene::addValue(const GLuint &value, GLindex *index)
{
    // return new index if pointer provided
    if (index != 0) {
        (*index) = (GLindex) m_values.size();
    }
    m_values.push_back(value);
    return (*this);
}

inline void GLElementDataGene::setFeatCount(const GLindex index, const GLuint &featcount)
{
    GLuint *data = reinterpret_cast<GLuint*>(&m_features[index]);
    (*data) = featcount;
}

inline void GLElementDataGene::setRefCount(const GLindex index, const GLuint &refcount)
{
    GLuint *data = reinterpret_cast<GLuint*>(&m_references[index]);
    (*data) = refcount;
}

inline void GLElementDataGene::setValue(const GLindex index, const GLuint &value)
{
    GLuint *data = reinterpret_cast<GLuint*>(&m_values[index]);
    (*data) = value;
}

inline GLuint GLElementDataGene::getFeatCount(const GLindex index) const
{
    return *(reinterpret_cast<const GLuint*>(&m_features[index]));
}

inline GLuint GLElementDataGene::getRefCount(const GLindex index) const
{
    return *(reinterpret_cast<const GLuint*>(&m_references[index]));
}

inline GLuint GLElementDataGene::getValue(const GLindex index) const
{
    return *(reinterpret_cast<const GLuint*>(&m_values[index]));
}

const GLarray<GLuint> GLElementDataGene::features() const
{
    return GLarray<GLuint>((GLsizei) m_features.size(), static_cast<const GLuint *>(m_features.data()));
}

const GLarray<GLuint> GLElementDataGene::references() const
{
    return GLarray<GLuint>((GLsizei) m_references.size(), static_cast<const GLuint *>(m_references.data()));
}

const GLarray<GLuint> GLElementDataGene::values() const
{
    return GLarray<GLuint>((GLsizei) m_values.size(), static_cast<const GLuint *>(m_values.data()));
}

} // namespace GL //

#endif // GLELEMENTDATAGENE_H
