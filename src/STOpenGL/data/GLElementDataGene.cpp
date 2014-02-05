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
    m_values(),
    m_options()
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
    if (flags & GLElementDataGene::OptionArray) {
        m_options.resize(0);
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
    std::fill(m_references.begin(), m_references.end(), 0u);
}

void GLElementDataGene::resetValCount()
{
    std::fill(m_values.begin(), m_values.end(), 0u);
}

void GLElementDataGene::resetFeatCount()
{
    std::fill(m_features.begin(), m_features.end(), 0u);
}

void GLElementDataGene::resetOptions()
{
    std::fill(m_options.begin(), m_options.end(), 0u);
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

void GLElementDataGene::addOption(const GLuint &option, GLindex *index)
{
    // return new index if pointer provided
    if (index != 0) {
        (*index) = (GLindex) m_options.size();
    }
    m_options.push_back(option);
}

void GLElementDataGene::setFeatCount(const GLindex &index, const GLuint &featcount)
{
    m_features[index] = featcount;
}

void GLElementDataGene::setRefCount(const GLindex &index, const GLuint &refcount)
{
    m_references[index] = refcount;
}

void GLElementDataGene::setValue(const GLindex &index, const GLuint &value)
{
    m_values[index] = value;
}

void GLElementDataGene::setOption(const GLindex &index, const GLuint &option)
{
    m_options[index] = option;
}

const GLuint& GLElementDataGene::getFeatCount(const GLindex &index) const
{
    return m_features[index];
}

const GLuint& GLElementDataGene::getRefCount(const GLindex &index) const
{
    return m_references[index];
}

const GLuint& GLElementDataGene::getValue(const GLindex &index) const
{
    return m_values[index];
}

GLuint GLElementDataGene::getOption(const GLindex &index) const
{
    return m_options[index];
}

const GLElementDataGene::FeaturesType &GLElementDataGene::features() const
{
    return m_features;
}

const GLElementDataGene::ReferencesType &GLElementDataGene::references() const
{
    return m_references;
}

const GLElementDataGene::ValuesType &GLElementDataGene::values() const
{
    return m_values;
}

const GLElementDataGene::OptionsType &GLElementDataGene::options() const
{
    return m_options;
}

} // namespace GL //
