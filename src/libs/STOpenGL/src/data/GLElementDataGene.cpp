/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GLElementDataGene.h"

namespace GL
{

GLElementDataGene::GLElementDataGene():
    GLElementData(),
    m_features(),
    m_references(),
    m_values()
{

}

GLElementDataGene::~GLElementDataGene()
{

}

void GLElementDataGene::clear(GLflag flags)
{

    if (flags & GLElementDataGene::FeatureCountArray) {
        m_features.resize(0);
    }
    if (flags & GLElementDataGene::RefCountArray) {
        m_references.resize(0);
    }
    if (flags & GLElementDataGene::ExpressionCountArray) {
        m_values.resize(0);
    }

    m_colorMode = Globals::NormalMode;
    m_geneMode = Globals::IndividualGeneMode;
    m_hitCountMin = Globals::gene_lower_limit;
    m_hitCountMax = Globals::gene_upper_limit;
    m_hitCountSum = Globals::gene_upper_limit;
    m_intensity = Globals::gene_intensity;
    m_geneLowerLimit = Globals::gene_threshold_min;
    m_geneUpperLimit = Globals::gene_threshold_max;
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

void GLElementDataGene::addFeatCount(const GLuint &featcount, GLindex *index)
{
    // return new index if pointer provided
    if (index != 0) {
        (*index) = (GLindex) m_features.size();
    }
    m_features.push_back(featcount);
}

void GLElementDataGene::addRefCount(const GLuint &refcount, GLindex *index)
{
    // return new index if pointer provided
    if (index != 0) {
        (*index) = (GLindex) m_references.size();
    }
    m_references.push_back(refcount);
}

void GLElementDataGene::addValue(const GLuint &value, GLindex *index)
{
    // return new index if pointer provided
    if (index != 0) {
        (*index) = (GLindex) m_values.size();
    }
    m_values.push_back(value);
}

void GLElementDataGene::setFeatCount(const GLindex &index, const GLuint &featcount)
{
    GLuint *data = reinterpret_cast<GLuint*>(&m_features[index]);
    (*data) = featcount;
}

void GLElementDataGene::setRefCount(const GLindex &index, const GLuint &refcount)
{
    GLuint *data = reinterpret_cast<GLuint*>(&m_references[index]);
    (*data) = refcount;
}

void GLElementDataGene::setValue(const GLindex &index, const GLuint &value)
{
    GLuint *data = reinterpret_cast<GLuint*>(&m_values[index]);
    (*data) = value;
}

const GLuint& GLElementDataGene::getFeatCount(const GLindex &index) const
{
    return *(reinterpret_cast<const GLuint*>(&m_features[index]));
}

const GLuint& GLElementDataGene::getRefCount(const GLindex &index) const
{
    return *(reinterpret_cast<const GLuint*>(&m_references[index]));
}

const GLuint& GLElementDataGene::getValue(const GLindex &index) const
{
    return *(reinterpret_cast<const GLuint*>(&m_values[index]));
}

const GLarray<GLuint> GLElementDataGene::features() const
{
    return GLarray<GLuint>((GLsizei) m_features.size(), m_features.constData() );
}

const GLarray<GLuint> GLElementDataGene::references() const
{
    return GLarray<GLuint>((GLsizei) m_references.size(), m_references.constData() );
}

const GLarray<GLuint> GLElementDataGene::values() const
{
    return GLarray<GLuint>((GLsizei) m_values.size(), m_values.constData() );
}

} // namespace GL //
