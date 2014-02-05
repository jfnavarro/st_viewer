/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GLTypeTraits.h"

#include "GLShaderRender.h"

namespace GL
{

GLShaderRender::GLShaderRender()
    : m_program(0)
{

}

GLShaderRender::~GLShaderRender()
{

}

void GLShaderRender::clear()
{

}

void GLShaderRender::shader(QGLShaderProgram *program)
{
    m_program = program;
}

void GLShaderRender::render(const GLElementDataGene& renderData)
{
    Q_ASSERT(m_program != 0 );

    const GLenum mode = renderData.mode();
    
    // primitive arrays
    const GLElementDataGene::VerticesType &vertices = renderData.vertices();
    const GLElementDataGene::ColorsType &colors = renderData.colors();
    const GLElementDataGene::IndexesType &indices = renderData.indices();
    const GLElementDataGene::TexturesType &textures = renderData.textures();
    const GLElementDataGene::OptionsType &options = renderData.options();

    // extra arrays
    const GLElementDataGene::FeaturesType &features = renderData.features();
    const GLElementDataGene::ReferencesType &references = renderData.references();
    const GLElementDataGene::ValuesType &values = renderData.values();

    const bool hasVertex = !vertices.isEmpty();
    const bool hasColors = !colors.isEmpty();
    const bool hasTexture = !textures.isEmpty();
    //const bool hasOptions = !options.isEmpty();
    const bool hasFeatures = !features.isEmpty();
    const bool hasReferences = !references.isEmpty();
    const bool hasValues = !values.isEmpty();
    const bool isGlobalMode = (renderData.getThresholdMode() == Globals::GlobalGeneMode);

    if (hasVertex && (!hasColors || !hasTexture)) {
        qDebug() << QString("Unable to render element data using shader "
                            "without colors||texture coordinates.");
        return;
    }

    if (isGlobalMode && (!hasFeatures || !hasReferences || !hasValues)) {
        qDebug() << QString("Unable to render element data using shader in "
                            "GlobalGeneMode without features||refereces||values.");
        return;
    }

    // set vertex array
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL::GLTypeTrait<GLfloat>::type_enum, 0, vertices.data());

    // set color array
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4, GL::GLTypeTrait<GLfloat>::type_enum, 0, colors.data());

    int textureLocation = m_program->attributeLocation("in_texture");
    m_program->setAttributeArray(textureLocation, GL::GLTypeTrait<GLfloat>::type_enum, textures.data(), 2);
    m_program->enableAttributeArray(textureLocation);

    int optionLocation = m_program->attributeLocation("in_options");
    m_program->setAttributeArray(optionLocation, GL::GLTypeTrait<GLuint>::type_enum, options.data(), 1);
    m_program->enableAttributeArray(optionLocation);

    // extra information vectors
    int featureLocation = m_program->attributeLocation("in_features");
    m_program->setAttributeArray(featureLocation, GL::GLTypeTrait<GLuint>::type_enum, features.data(), 1);
    m_program->enableAttributeArray(featureLocation);


    int referenceLocation = m_program->attributeLocation("in_references");
    m_program->setAttributeArray(referenceLocation, GL::GLTypeTrait<GLuint>::type_enum, references.data(), 1);
    m_program->enableAttributeArray(referenceLocation);
    int valueLocation = m_program->attributeLocation("in_values");
    m_program->setAttributeArray(valueLocation, GL::GLTypeTrait<GLuint>::type_enum, values.data(), 1);
    m_program->enableAttributeArray(valueLocation);

    // uniform values
    int colorMode = m_program->uniformLocation("in_colorMode");
    int colorModeValue = static_cast<int>(renderData.getColorMode());
    m_program->setUniformValue(colorMode, colorModeValue);

    int geneMode = m_program->uniformLocation("in_geneMode");
    int geneModeValue = static_cast<int>(renderData.getThresholdMode());
    m_program->setUniformValue(geneMode, geneModeValue);

    int hitCountLocationMin = m_program->uniformLocation("in_hitCountMin");
    m_program->setUniformValue(hitCountLocationMin, renderData.getHitCountMin());

    int hitCountLocationMax = m_program->uniformLocation("in_hitCountMax");
    m_program->setUniformValue(hitCountLocationMax, renderData.getHitCountMax());

    int hitCountLocationSum = m_program->uniformLocation("in_hitCountSum");
    m_program->setUniformValue(hitCountLocationSum, renderData.getHitCountSum());

    int upperLimit = m_program->uniformLocation("in_upper");
    m_program->setUniformValue(upperLimit ,renderData.getUpperLimit());

    int lowerLimit = m_program->uniformLocation("in_lower");
    m_program->setUniformValue(lowerLimit, renderData.getLowerLimit());

    int intensity = m_program->uniformLocation("in_intensity");
    m_program->setUniformValue(intensity, renderData.getIntensity());

    /*qDebug() << "Shader Renderer Uniform Values : hitCoutMin = " << renderData.getHitCountMin()
                << " hitCountMax = " << renderData.getHitCountMax() << " hitCountSum = " << renderData.getHitCountSum()
                   << " Upper Limit = " << renderData.getUpperLimit() << " LowerLimit = " << renderData.getLowerLimit()
                      << " Intensity = " << renderData.getIntensity() << " Gene Mode = " << geneModeValue << " Color Mode = " << colorModeValue;*/
    // draw call
    glDrawElements(mode, static_cast<GLsizei>(indices.size()),
                   GL::GLTypeTrait<GLindex>::type_enum, indices.data());

    m_program->disableAttributeArray(textureLocation);
    m_program->disableAttributeArray(optionLocation);
    m_program->disableAttributeArray(featureLocation);
    m_program->disableAttributeArray(referenceLocation);
    m_program->disableAttributeArray(valueLocation);

    // unset color array
    glDisableClientState(GL_COLOR_ARRAY);
    // unset vertex array
    glDisableClientState(GL_VERTEX_ARRAY);
    
}

} // namespace GL //
