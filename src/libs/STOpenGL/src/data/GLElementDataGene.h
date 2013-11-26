#ifndef GLELEMENTDATAGENE_H
#define GLELEMENTDATAGENE_H

#include "GLElementData.h"

namespace GL
{

class GLElementDataGene : public GLElementData
{
public:

    enum ClearFlags {
        PointArray = 0x001u,
        ColorArray = 0x002u,
        IndexArray = 0x004u,
        TextureArray = 0x008u,
        OptionArray = 0x010u,
        FeaturesArray = 0x12u,
        RefCountArray = 0x14u,
        ValueArray = 0x16u,
        GeneOptionArray = 0x18u,
        RenderMode = 0x100u,
        // composite flags
        Arrays = (PointArray | ColorArray | IndexArray | TextureArray | OptionArray
        | FeaturesArray | RefCountArray | ValueArray | GeneOptionArray),
        Modes = (RenderMode),
        All = (Arrays | Modes)
    };

    inline GLElementDataGene();

    //clear arrays
    inline void clear(GLflag flags = DEFAULT_CLEAR_FLAGS);

    inline void resetRefCount();

    //visibles
    inline GLElementDataGene &addFeatCount(const GLuint &featcount, GLindex *index = 0);
    //refcounts
    inline GLElementDataGene &addRefCount(const GLuint &refcount, GLindex *index = 0);
    //values
    inline GLElementDataGene &addValue(const GLuint &value, GLindex *index = 0);
    //options
    inline GLElementDataGene &addGeneOption(const GLoption &option, GLindex *index = 0);

    inline void setFeatCount(const GLindex index, const GLuint &featcount);
    inline void setRefCount(const GLindex index, const GLuint &refcount);
    inline void setValue(const GLindex index, const GLuint &value);
    inline void setGeneOption(const GLindex index, const GLoption &option);

    inline const GLuint getFeatCount(const GLindex index);
    inline const GLuint getRefCount(const GLindex index);
    inline const GLuint getValue(const GLindex index);
    inline const GLoption getGeneOption(const GLindex index);

    inline const GLarray<GLuint> features() const;
    inline const GLarray<GLuint> references() const;
    inline const GLarray<GLuint> values() const;
    inline const GLarray<GLoption> geneOptions() const;

    //check whether the arrays are empty or not
    inline bool isEmpty() const
    {
        return m_colors.empty() && m_indices.empty() && m_textures.empty()
                && m_points.empty() && m_options.empty() && m_features.empty()
                && m_references.empty() && m_values.empty() && m_geneOptions.empty();
    }

private:

    typedef QVector<GLuint> GLFeatures;
    typedef QVector<GLuint> GLReferences;
    typedef QVector<GLuint> GLValues;
    typedef QVector<GLoption> GLGeneOptions;

    GLFeatures m_features;
    GLReferences m_references;
    GLValues m_values;
    GLGeneOptions m_geneOptions;
};

} // namespace GL //

/****************************************** DEFINITION ******************************************/

namespace GL
{


GLElementDataGene::GLElementDataGene():
    GLElementData(), m_features(), m_references(), m_values(), m_geneOptions()
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
    if (flags & GLElementDataGene::GeneOptionArray) {
        m_geneOptions.resize(0);
    }

    GLElementData::clear();
}

void GLElementDataGene::resetRefCount()
{
    for (GLindex index = 0; index < (GLindex) m_references.size(); index++) {
        GLuint *data = reinterpret_cast<GLuint*>(&m_references[index]);
        (*data) = 0u;
    }
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

inline GLElementDataGene &GLElementDataGene::addGeneOption(const GLoption &option, GLindex *index)
{
    // return new index if pointer provided
    if (index != 0) {
        (*index) = (GLindex) m_geneOptions.size();
    }
    m_geneOptions.push_back(option);
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

inline void GLElementDataGene::setGeneOption(const GLindex index, const GLoption &option)
{
    GLoption *data = reinterpret_cast<GLoption*>(&m_geneOptions[index]);
    (*data) = option;
}

inline const GLuint GLElementDataGene::getFeatCount(const GLindex index)
{
    return *(reinterpret_cast<GLuint*>(&m_features[index]));
}

inline const GLuint GLElementDataGene::getRefCount(const GLindex index)
{
    return *(reinterpret_cast<GLuint*>(&m_references[index]));
}

inline const GLuint GLElementDataGene::getValue(const GLindex index)
{
    return *(reinterpret_cast<GLuint*>(&m_values[index]));
}

inline const GLoption GLElementDataGene::getGeneOption(const GLindex index)
{
    return *(reinterpret_cast<GLoption*>(&m_geneOptions[index]));
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

const GLarray<GLoption> GLElementDataGene::geneOptions() const
{
    return GLarray<GLoption>((GLsizei) m_geneOptions.size(), static_cast<const GLoption *>(m_geneOptions.data()));
}

} // namespace GL //

#endif // GLELEMENTDATAGENE_H
