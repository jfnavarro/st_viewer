/*
Copyright (C) 2012  Spatial Transcriptomics AB,
read LICENSE for licensing terms.
Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "Renderer.h"
#include "ColoredLines.h"
#include "ColoredQuads.h"
#include "STTexturedQuads.h"
#include "AssertOpenGL.h"

#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QPointer>
#include <QtGlobal>
#include <stdexcept>
#include <map>

static_assert(sizeof(GLfloat) == sizeof(float), "GLfloat and float are not same size.");

class Renderer::Internals
{
public:
    Internals();

    ~Internals();

    void draw(const QMatrix4x4& transform, const ColoredLines& lines);

    void draw(const QMatrix4x4& transform, const ColoredQuads& quads);

    void draw(const QMatrix4x4& transform,
              const QString& textureName,
              const STTexturedQuads& quads);

    void addTexture(const QString& name, QSharedPointer<QOpenGLTexture> texture);

    void addTexture(const QString& name, const QImage& image, const bool makeMipMaps);

    void removeTexture(const QString& name);

private:
    struct BlendStates {
        // The default constructed BlendStates matches the OpenGL defaults.
        BlendStates();
        BlendStates(const GLboolean enable, const GLint src, const GLint dst);
        bool operator!=(const BlendStates& rhs) const;
        GLint blendSrc;
        GLint blendDst;
        GLboolean blendingEnabled;
    };

    struct LineStates {
        // The default constructed LineStates matches the OpenGL defaults.
        LineStates();
        LineStates(const GLboolean smooth, const GLfloat width);
        bool operator!=(const LineStates& rhs) const;
        GLfloat lineWidth;
        GLboolean smoothingEnabled;
    };

    // Asserts that the current OpenGL context is available and no in an error state,
    // and returns a valid pointer to it.
    QOpenGLFunctions* getOpenGLFunctions() const;

    // Returns the currently set blend state variables that the renderer is interested in.
    BlendStates getBlendStates() const;

    // Returns the currently set line state variables that the renderer is interested in.
    LineStates getLineStates() const;

    // Sets the OpenGL blend state to match states.
    void setBlendStates(const BlendStates& states);

    // Sets the OpenGL blend state to match states.
    void setLineStates(const LineStates& states);

    QSharedPointer<QOpenGLTexture> getTexture(const QString& name);

    QPointer<QOpenGLShaderProgram> m_colorProgram;
    QPointer<QOpenGLShaderProgram> m_textureProgram;
    std::map<QString, QSharedPointer<QOpenGLTexture>> m_namedTextures;
    QSharedPointer<QOpenGLTexture> m_defaultTexture;
    int m_attrTexUv;
    int m_attrTexPoint;
    int m_attrTexBgra;
    int m_uniTexTransform;
    int m_uniTexSampler;
    int m_attrColorPoint;
    int m_attrColorBgra;
    int m_uniColorTransform;
};

namespace
{

// Returns a shared pointer to a texture constructed from image. If makeMipMaps is true, the
// texture will have trilinear filtered mip maps, if false no mipmaps and nearest point sampling.
QSharedPointer<QOpenGLTexture> makeIntoTexture(const QImage& image, const bool makeMipMaps)
{
    const auto mipsOption
        = makeMipMaps ? QOpenGLTexture::GenerateMipMaps : QOpenGLTexture::DontGenerateMipMaps;

    QSharedPointer<QOpenGLTexture> texture(new QOpenGLTexture(image.mirrored(), mipsOption));

    if (mipsOption) {
        texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        texture->setMagnificationFilter(QOpenGLTexture::Linear);
    } else {
        texture->setMinificationFilter(QOpenGLTexture::Nearest);
        texture->setMagnificationFilter(QOpenGLTexture::Nearest);
    }

    texture->setWrapMode(QOpenGLTexture::ClampToEdge);

    return texture;
}

// Returns a yellow 256 x 256 texture with no mipmaps.
QSharedPointer<QOpenGLTexture> makeDefaultTexture()
{
    const int sideLength = 256;
    QSharedPointer<QImage> image(new QImage(sideLength, sideLength, QImage::Format_ARGB32));
    image->fill(Qt::yellow);
    return makeIntoTexture(*image, false);
}

// Compiled and links the vertex shader source vs and the fragment shader source fs into an OpenGL
// program, and returns it as a pointer.
QPointer<QOpenGLShaderProgram> makeProgram(const char* vs, const char* fs)
{
    QPointer<QOpenGLShaderProgram> program = new QOpenGLShaderProgram();

    const bool vsOK = program->addShaderFromSourceCode(QOpenGLShader::Vertex, vs);
    const bool fsOK = program->addShaderFromSourceCode(QOpenGLShader::Fragment, fs);
    const bool linkOK = program->link();

    if (!(vsOK && fsOK && linkOK)) {
        delete program;
        qDebug() << "Error creating QOpenGLShaderProgram.";
        throw std::runtime_error("Error creating QOpenGLShaderProgram.");
    }

    return program;
}

QPointer<QOpenGLShaderProgram> makeColorProgram()
{
    const char* vertexShader = ""
                               "uniform mat4 transform;   \n"
                               "attribute vec2 point;     \n"
                               "attribute vec4 pointBGRA; \n"
                               "varying vec4 col;         \n"
                               "void main()               \n"
                               "{                         \n"
                               "   col.bgra = pointBGRA;  \n"
                               "   gl_Position = transform * vec4(point, 0, 1); \n"
                               "}";

    const char* fragmentShader = ""
                                 "varying vec4 col;      \n"
                                 "void main()            \n"
                                 "{                      \n"
                                 "   gl_FragColor = col; \n"
                                 "}";

    return makeProgram(vertexShader, fragmentShader);
}

QPointer<QOpenGLShaderProgram> makeTexProgram()
{
    const char* vertexShader = ""
                               "uniform mat4 transform;                         \n"
                               "attribute vec2 point;                           \n"
                               "attribute vec2 uv;                              \n"
                               "attribute vec4 pointBGRA;                       \n"
                               "varying vec4 col;                               \n"
                               "varying vec2 texCoord;                          \n"
                               "void main()                                     \n"
                               "{                                               \n"
                               "   texCoord = uv;                               \n"
                               "   col.bgra = pointBGRA;                        \n"
                               "   gl_Position = transform * vec4(point, 0, 1); \n"
                               "}";

    const char* fragmentShader = ""
                                 "uniform sampler2D tex;                          \n"
                                 "varying vec2 texCoord;                          \n"
                                 "varying vec4 col;                               \n"
                                 "void main()                                     \n"
                                 "{                                               \n"
                                 "   vec4 texColor = texture(tex,texCoord);       \n"
                                 "   gl_FragColor = col * texColor;               \n"
                                 "}";

    return makeProgram(vertexShader, fragmentShader);
}
}

Renderer::Internals::Internals()
    : m_colorProgram(makeColorProgram())
    , m_textureProgram(makeTexProgram())
    , m_namedTextures()
    , m_defaultTexture(makeDefaultTexture())
    , m_attrTexUv(-1)
    , m_attrTexPoint(-1)
    , m_attrTexBgra(-1)
    , m_uniTexTransform(-1)
    , m_uniTexSampler(-1)
    , m_attrColorPoint(-1)
    , m_attrColorBgra(-1)
    , m_uniColorTransform(-1)
{
    Q_ASSERT(QOpenGLContext::currentContext() != nullptr);

    m_attrTexPoint = m_textureProgram->attributeLocation("point");
    m_attrTexBgra = m_textureProgram->attributeLocation("pointBGRA");
    m_uniTexTransform = m_textureProgram->uniformLocation("transform");
    m_uniTexSampler = m_textureProgram->uniformLocation("tex");
    m_attrTexUv = m_textureProgram->attributeLocation("uv");

    m_attrColorPoint = m_colorProgram->attributeLocation("point");
    m_attrColorBgra = m_colorProgram->attributeLocation("pointBGRA");
    m_uniColorTransform = m_colorProgram->uniformLocation("transform");
}

Renderer::Internals::~Internals()
{
    m_namedTextures.clear();
    m_defaultTexture.clear();

    delete m_textureProgram;
    m_textureProgram = nullptr;

    delete m_colorProgram;
    m_colorProgram = nullptr;
}

Renderer::Internals::BlendStates::BlendStates()
    : blendSrc(GL_ONE)
    , blendDst(GL_ZERO)
    , blendingEnabled(GL_FALSE)
{
}

Renderer::Internals::BlendStates::BlendStates(const GLboolean enable,
                                              const GLint src,
                                              const GLint dst)
    : blendSrc(src)
    , blendDst(dst)
    , blendingEnabled(enable)
{
}

bool Renderer::Internals::BlendStates::operator!=(const Renderer::Internals::BlendStates& rhs) const
{
    return (blendSrc != rhs.blendSrc) || (blendDst != rhs.blendDst)
           || (blendingEnabled != rhs.blendingEnabled);
}

Renderer::Internals::LineStates::LineStates()
    : lineWidth(1.0f)
    , smoothingEnabled(GL_FALSE)
{
}

Renderer::Internals::LineStates::LineStates(const GLboolean smooth, const GLfloat width)
    : lineWidth(width)
    , smoothingEnabled(smooth)
{
}

bool Renderer::Internals::LineStates::operator!=(const Renderer::Internals::LineStates& rhs) const
{
    return (lineWidth != rhs.lineWidth) || (smoothingEnabled != rhs.smoothingEnabled);
}

QOpenGLFunctions* Renderer::Internals::getOpenGLFunctions() const
{
    Q_ASSERT(QOpenGLContext::currentContext() != nullptr);
    ASSERT_OPENGL_OK;

    QOpenGLFunctions* funcs = QOpenGLContext::currentContext()->functions();
    Q_ASSERT(funcs);

    return funcs;
}

void Renderer::Internals::setBlendStates(const BlendStates& states)
{

    if (states.blendingEnabled) {
        getOpenGLFunctions()->glEnable(GL_BLEND);
    } else {
        getOpenGLFunctions()->glDisable(GL_BLEND);
    }

    getOpenGLFunctions()->glBlendFunc(states.blendSrc, states.blendDst);
}

Renderer::Internals::BlendStates Renderer::Internals::getBlendStates() const
{
    auto funcs = getOpenGLFunctions();

    Renderer::Internals::BlendStates states;
    states.blendingEnabled = funcs->glIsEnabled(GL_BLEND);
    funcs->glGetIntegerv(GL_BLEND_SRC, &states.blendSrc);
    funcs->glGetIntegerv(GL_BLEND_DST, &states.blendDst);

    return states;
}

void Renderer::Internals::setLineStates(const LineStates& states)
{
    auto funcs = getOpenGLFunctions();

    if (states.smoothingEnabled) {
        funcs->glEnable(GL_LINE_SMOOTH);
    } else {
        funcs->glDisable(GL_LINE_SMOOTH);
    }

    funcs->glLineWidth(states.lineWidth);
}

Renderer::Internals::LineStates Renderer::Internals::getLineStates() const
{
    auto funcs = getOpenGLFunctions();

    Renderer::Internals::LineStates states;
    states.smoothingEnabled = funcs->glIsEnabled(GL_LINE_SMOOTH);
    funcs->glGetFloatv(GL_LINE_WIDTH, &states.lineWidth);

    return states;
}

void Renderer::Internals::draw(const QMatrix4x4& transform, const ColoredLines& lines)
{
    auto funcs = getOpenGLFunctions();

    // Find out what the current OpenGL context state is, so we can reset it afterwards.

    const auto originalLineStates = getLineStates();
    const auto originalBlendStates = getBlendStates();

    const BlendStates classicBlending(GL_TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    const LineStates smoothLine(GL_TRUE, lines.getLineWidth());

    if (originalLineStates != smoothLine) {
        setLineStates(smoothLine);
    }

    if (classicBlending != originalBlendStates) {
        setBlendStates(classicBlending);
    }

    m_colorProgram->bind();
    m_colorProgram->setUniformValue(m_uniColorTransform, transform);

    const auto pointsBuffer = lines.m_lines.data();
    const auto colorsBuffer = lines.m_colors.data();

    funcs->glEnableVertexAttribArray(0);
    funcs->glEnableVertexAttribArray(1);

    funcs->glVertexAttribPointer(m_attrColorPoint, 2, GL_FLOAT, GL_FALSE, 0, pointsBuffer);
    funcs->glVertexAttribPointer(m_attrColorBgra, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, colorsBuffer);

    funcs->glDrawArrays(GL_LINES, 0, 2u * static_cast<GLsizei>(lines.lineCount()));

    funcs->glDisableVertexAttribArray(1);
    funcs->glDisableVertexAttribArray(0);

    m_colorProgram->release();

    // Reset the OpenGL context states.

    if (smoothLine != originalLineStates) {
        setLineStates(originalLineStates);
    }

    if (classicBlending != originalBlendStates) {
        setBlendStates(originalBlendStates);
    }

    ASSERT_OPENGL_OK;
}

void Renderer::Internals::draw(const QMatrix4x4& transform, const ColoredQuads& quads)
{
    auto funcs = getOpenGLFunctions();

    // Find out what the current OpenGL context state is, so we can reset it afterwards.

    const auto originalBlendStates = getBlendStates();

    const BlendStates classicBlending(GL_TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (classicBlending != originalBlendStates) {
        setBlendStates(classicBlending);
    }

    m_colorProgram->bind();
    m_colorProgram->setUniformValue(m_uniColorTransform, transform);

    const auto vertexBuffer = quads.m_quads.data();
    const auto colorsBuffer = quads.m_quadColors.data();

    funcs->glEnableVertexAttribArray(0);
    funcs->glEnableVertexAttribArray(1);
    funcs->glDisableVertexAttribArray(2);

    funcs->glVertexAttribPointer(m_attrColorPoint, 2, GL_FLOAT, GL_FALSE, 0, vertexBuffer);
    funcs->glVertexAttribPointer(m_attrColorBgra, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, colorsBuffer);

    funcs->glDrawArrays(GL_TRIANGLES, 0, 6u * static_cast<GLsizei>(quads.quadCount()));

    funcs->glDisableVertexAttribArray(1);
    funcs->glDisableVertexAttribArray(0);

    m_colorProgram->release();

    // Reset the OpenGL context states.

    if (classicBlending != originalBlendStates) {
        setBlendStates(originalBlendStates);
    }

    ASSERT_OPENGL_OK;
}

void Renderer::Internals::draw(const QMatrix4x4& transform,
                               const QString& textureName,
                               const STTexturedQuads& quads)
{
    auto funcs = getOpenGLFunctions();

    auto texture = getTexture(textureName);

    // Find out what the current OpenGL context state is, so we can reset it afterwards.

    const auto originalBlendStates = getBlendStates();

    const BlendStates classicBlending(GL_TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (classicBlending != originalBlendStates) {
        setBlendStates(classicBlending);
    }

    m_textureProgram->bind();
    m_textureProgram->setUniformValue(m_uniTexTransform, transform);
    m_textureProgram->setUniformValue(m_uniTexSampler, 0);

    const auto vertexBuffer = quads.m_quads.m_quads.data();
    const auto uvBuffer = quads.m_textureCoords.data();
    const auto colorsBuffer = quads.m_quads.m_quadColors.data();

    funcs->glEnableVertexAttribArray(0);
    funcs->glEnableVertexAttribArray(1);
    funcs->glEnableVertexAttribArray(2);

    funcs->glVertexAttribPointer(m_attrTexPoint, 2, GL_FLOAT, GL_FALSE, 0, vertexBuffer);
    funcs->glVertexAttribPointer(m_attrTexUv, 2, GL_FLOAT, GL_FALSE, 0, uvBuffer);
    funcs->glVertexAttribPointer(m_attrTexBgra, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, colorsBuffer);

    texture->bind(0);

    funcs->glDrawArrays(GL_TRIANGLES, 0, 6u * static_cast<GLsizei>(quads.quadCount()));

    texture->release(0);

    funcs->glDisableVertexAttribArray(2);
    funcs->glDisableVertexAttribArray(1);
    funcs->glDisableVertexAttribArray(0);

    m_textureProgram->release();

    // Reset the OpenGL context states.

    if (classicBlending != originalBlendStates) {
        setBlendStates(originalBlendStates);
    }

    ASSERT_OPENGL_OK;
}

QSharedPointer<QOpenGLTexture> Renderer::Internals::getTexture(const QString& name)
{
    QSharedPointer<QOpenGLTexture> result;

    auto tex = m_namedTextures.find(name);

    // Return the default texture if the named on cannot be found. This means that no
    // textured drawing operation ever fails.
    if (tex == m_namedTextures.cend()) {
        result = m_defaultTexture;
        qDebug() << "Warning: texture '" << name << "' was not found. "
                 << "Using default (yellow) texture instead.";
    } else {
        result = tex->second;
    }

    return result;
}

void Renderer::Internals::addTexture(const QString& name, QSharedPointer<QOpenGLTexture> texture)
{
    m_namedTextures[name] = texture;
}

void Renderer::Internals::addTexture(const QString& name,
                                     const QImage& image,
                                     const bool makeMipMaps)
{
    auto tex = makeIntoTexture(image, makeMipMaps);
    addTexture(name, tex);
}

void Renderer::Internals::removeTexture(const QString& name)
{
    m_namedTextures.erase(name);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

Renderer::Renderer()
    : m_internals(std::make_shared<Renderer::Internals>())
{
    ASSERT_OPENGL_OK;
}

void Renderer::draw(const QMatrix4x4& transform, const ColoredLines& lines)
{
    m_internals->draw(transform, lines);
}

void Renderer::draw(const QMatrix4x4& transform, const ColoredQuads& quads)
{
    m_internals->draw(transform, quads);
}

void Renderer::draw(const QMatrix4x4& transform,
                    const QString& textureName,
                    const STTexturedQuads& quads)
{
    m_internals->draw(transform, textureName, quads);
}

void Renderer::addTexture(const QString& name, const QImage& image, const bool makeMipMaps)
{
    m_internals->addTexture(name, image, makeMipMaps);
}

void Renderer::addTexture(const QString& name, QSharedPointer<QOpenGLTexture> texture)
{
    m_internals->addTexture(name, texture);
}

void Renderer::removeTexture(const QString& name)
{
    m_internals->removeTexture(name);
}
