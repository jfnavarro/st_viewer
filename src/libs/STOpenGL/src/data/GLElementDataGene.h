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
        VisibleArray = 0x12u,
        RefCountArray = 0x14u,
        ValueArray = 0x16u,
        RenderMode = 0x100u,
        // composite flags
        Arrays = (PointArray | ColorArray | IndexArray | TextureArray | OptionArray
        | VisibleArray | RefCountArray | ValueArray),
        Modes = (RenderMode),
        All = (Arrays | Modes)
    };

    inline GLElementDataGene();

    //clear arrays
    inline void clear(GLflag flags = DEFAULT_CLEAR_FLAGS);

    //visibles
    inline GLElementDataGene &addVisible(const GLoption &visible, GLindex *index = 0);
    //refcounts
    inline GLElementDataGene &addRefCount(const GLindex &refcount, GLindex *index = 0);
    //values
    inline GLElementDataGene &addValue(const GLindex &value, GLindex *index = 0);

    inline void setVisible(const GLindex index, const GLoption &visible);
    inline void setRefCount(const GLindex index, const GLindex &refcount);
    inline void setValue(const GLindex index, const GLindex &value);

    inline const GLoption getVisible(const GLindex index);
    inline const GLindex getRefCount(const GLindex index);
    inline const GLindex getValue(const GLindex index);

    inline const GLarray<GLoption> visibles() const;
    inline const GLarray<GLindex> references() const;
    inline const GLarray<GLindex> values() const;

    //check whether the arrays are empty or not
    inline bool isEmpty() const
    {
        return m_colors.empty() && m_indices.empty() && m_textures.empty()
                && m_points.empty() && m_options.empty() && m_visibles.empty() && m_references.empty() && m_values.empty();
    }

private:

    typedef QVector<GLoption> GLVisibles;
    typedef QVector<GLindex> GLReferences;
    typedef QVector<GLindex> GLValues;

    GLVisibles m_visibles;
    GLReferences m_references;
    GLValues m_values;
};

} // namespace GL //

/****************************************** DEFINITION ******************************************/

namespace GL
{


GLElementDataGene::GLElementDataGene(): GLElementData(), m_visibles(), m_references(), m_values()
{

}

void GLElementDataGene::clear(GLflag flags)
{

    if (flags & GLElementDataGene::VisibleArray) {
        m_visibles.resize(0);
    }
    if (flags & GLElementDataGene::RefCountArray) {
        m_references.resize(0);
    }
    if (flags & GLElementDataGene::ValueArray) {
        m_values.resize(0);
    }

    GLElementData::clear();
}

inline GLElementDataGene &GLElementDataGene::addVisible(const GLoption &option, GLindex *index)
{
    // return new index if pointer provided
    if (index != 0) {
        (*index) = (GLindex) m_visibles.size();
    }
    m_visibles.push_back(option);
    return (*this);
}

inline GLElementDataGene &GLElementDataGene::addRefCount(const GLindex &refcount, GLindex *index)
{
    // return new index if pointer provided
    if (index != 0) {
        (*index) = (GLindex) m_references.size();
    }
    m_references.push_back(refcount);
    return (*this);
}

inline GLElementDataGene &GLElementDataGene::addValue(const GLindex &value, GLindex *index)
{
    // return new index if pointer provided
    if (index != 0) {
        (*index) = (GLindex) m_values.size();
    }
    m_values.push_back(value);
    return (*this);
}

inline void GLElementDataGene::setVisible(const GLindex index, const GLoption &option)
{
    GLoption *data = reinterpret_cast<GLoption*>(&m_visibles[index]);
    (*data) = option;
}

inline void GLElementDataGene::setRefCount(const GLindex index, const GLindex &refcount)
{
    GLindex *data = reinterpret_cast<GLindex*>(&m_references[index]);
    (*data) = refcount;
}

inline void GLElementDataGene::setValue(const GLindex index, const GLindex &value)
{
    GLindex *data = reinterpret_cast<GLindex*>(&m_values[index]);
    (*data) = value;
}

inline const GLoption GLElementDataGene::getVisible(const GLindex index)
{
    return *(reinterpret_cast<GLoption*>(&m_visibles[index]));
}

inline const GLindex GLElementDataGene::getRefCount(const GLindex index)
{
    return *(reinterpret_cast<GLindex*>(&m_references[index]));
}

inline const GLindex GLElementDataGene::getValue(const GLindex index)
{
    return *(reinterpret_cast<GLindex*>(&m_values[index]));
}

const GLarray<GLoption> GLElementDataGene::visibles() const
{
    return GLarray<GLoption>((GLsizei) m_visibles.size(), static_cast<const GLoption *>(m_visibles.data()));
}

const GLarray<GLindex> GLElementDataGene::references() const
{
    return GLarray<GLindex>((GLsizei) m_references.size(), static_cast<const GLindex *>(m_references.data()));
}

const GLarray<GLindex> GLElementDataGene::values() const
{
    return GLarray<GLindex>((GLsizei) m_values.size(), static_cast<const GLindex *>(m_values.data()));
}

} // namespace GL //

#endif // GLELEMENTDATAGENE_H
