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
    const GLenum mode = renderData.mode();
    
    // primitive arrays
    const GLarray<GLpoint> vertices = renderData.vertices();
    const GLarray<GLcolor> colors = renderData.colors();
    const GLarray<GLindex> indices = renderData.indices();
    const GLarray<GLpoint> textures = renderData.textures();
    const GLarray<GLoption> options = renderData.options();

    // extra arrays
    const GLarray<GLuint> features = renderData.features();
    const GLarray<GLuint> references = renderData.references();
    const GLarray<GLuint> values = renderData.values();

    const bool hasVertex = (vertices.size != 0);
    const bool hasColors = (colors.size != 0);
    const bool hasTexture = (textures.size != 0);
    const bool hasOptions = (options.size != 0);
    const bool hasFeatures = (features.size != 0);
    const bool hasReferences = (references.size != 0);
    const bool hasValues = (values.size != 0);

    const bool isGlobalMode = (renderData.getThresholdMode() == Globals::GlobalGeneMode);

    if (hasVertex && (!hasColors || !hasTexture || !hasOptions)) {
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
    glVertexPointer(2, GL::GLTypeTrait<GLfloat>::type_enum, 0, vertices.data);
    // set color array
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4, GL::GLTypeTrait<GLfloat>::type_enum, 0, colors.data);

    int textureLocation = -1;
    int optionLocation = -1;
    int featureLocation = -1;
    int referenceLocation = -1;
    int valueLocation = -1;
    int hitCountLocationMin = -1;
    int hitCountLocationMax = -1;
    int hitCountLocationSum = -1;
    int upperLimit = -1;
    int lowerLimit = -1;
    int colorMode = -1;
    int geneMode = -1;
    int intensity = -1;

    qDebug() << "Shaders, rendering genes with min : " << renderData.getMin() << " max : " << renderData.getMax()
                << " sum : " << renderData.getSum() << " upper : " << renderData.getUpperLimit() << " lower : " << renderData.getLowerLimit();
    if (m_program != 0) {
        // enable attribute arrays

        textureLocation = m_program->attributeLocation("in_texture");
        m_program->setAttributeArray(textureLocation, GL::GLTypeTrait<GLfloat>::type_enum,
                                     (const void *) textures.data, 2);
        m_program->enableAttributeArray(textureLocation);

        optionLocation = m_program->attributeLocation("in_options");
        m_program->setAttributeArray(optionLocation, GL::GLTypeTrait<GLoption>::type_enum,
                                     (const void *) options.data, 1);
        m_program->enableAttributeArray(optionLocation);

        // extra information vectors
        featureLocation = m_program->attributeLocation("in_features");
        m_program->setAttributeArray(featureLocation, GL::GLTypeTrait<GLfloat>::type_enum,
                                     (const void *) features.data, 1);
        m_program->enableAttributeArray(featureLocation);

        referenceLocation = m_program->attributeLocation("in_references");
        m_program->setAttributeArray(referenceLocation, GL::GLTypeTrait<GLfloat>::type_enum,
                                     (const void *) references.data, 1);
        m_program->enableAttributeArray(referenceLocation);

        valueLocation = m_program->attributeLocation("in_values");
        m_program->setAttributeArray(valueLocation, GL::GLTypeTrait<GLfloat>::type_enum,
                                     (const void *) values.data, 1);
        m_program->enableAttributeArray(valueLocation);

        // uniform values
        colorMode = m_program->uniformLocation("in_colorMode");
        int colorModeValue = static_cast<int>(renderData.getColorMode());
        m_program->setUniformValue(colorMode,colorModeValue);

        geneMode = m_program->uniformLocation("in_geneMode");
        int geneModeValue = static_cast<int>(renderData.getThresholdMode());
        m_program->setUniformValue(geneMode,geneModeValue);

        hitCountLocationMin = m_program->uniformLocation("in_hitCountMin");
        m_program->setUniformValue(hitCountLocationMin, renderData.getMin());

        hitCountLocationMax = m_program->uniformLocation("in_hitCountMax");
        m_program->setUniformValue(hitCountLocationMax, renderData.getMax());

        hitCountLocationSum = m_program->uniformLocation("in_hitCountSum");
        m_program->setUniformValue(hitCountLocationSum,renderData.getSum());

        upperLimit = m_program->uniformLocation("in_upper");
        m_program->setUniformValue(upperLimit,renderData.getUpperLimit());

        lowerLimit = m_program->uniformLocation("in_lower");
        m_program->setUniformValue(lowerLimit,renderData.getLowerLimit());

        intensity = m_program->uniformLocation("in_intensity");
        m_program->setUniformValue(intensity,renderData.getIntensity());

    }

    // draw call
    glDrawElements(mode, indices.size, GL::GLTypeTrait<GLindex>::type_enum, indices.data);

    if (m_program != 0) {
        m_program->disableAttributeArray(textureLocation);
        m_program->disableAttributeArray(optionLocation);
        m_program->disableAttributeArray(featureLocation);
        m_program->disableAttributeArray(referenceLocation);
        m_program->disableAttributeArray(valueLocation);
    }
    
    // unset color array
    glDisableClientState(GL_COLOR_ARRAY);
    // unset vertex array
    glDisableClientState(GL_VERTEX_ARRAY);
    
}

} // namespace GL //
