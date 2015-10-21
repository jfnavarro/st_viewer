/*
Copyright (C) 2012  Spatial Transcriptomics AB,
read LICENSE for licensing terms.
Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "Renderer.h"
#include "ColoredLines.h"
#include "ColoredQuads.h"
#include "TexturedQuads.h"
#include "AssertOpenGL.h"

#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_2_Compatibility>
#include <QOpenGLTexture>
#include <QPointer>
#include <QtGlobal>
#include <stdexcept>
#include <memory>
#include <map>

static_assert(sizeof(GLfloat) == sizeof(float), "GLfloat and float are not same size.");

static const GLuint VA_POINTS = 0;
static const GLuint VA_UVS = 1;
static const GLuint VA_COLORS = 2;

typedef QOpenGLFunctions_3_2_Compatibility  GLFuncsQt_t;

class Renderer::Internals
{
public:
    explicit Internals(std::unique_ptr<GLFuncsQt_t>);

    ~Internals();

    void draw(const QMatrix4x4& transform, const ColoredLines& lines);

    void draw(const QMatrix4x4& transform, const ColoredQuads& quads);

    void draw(const QMatrix4x4& transform, const QString& textureName, const TexturedQuads& quads);

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

    // Makes the three vertex buffer objects (vertices/colors/texture coords).
    // Called only by the ctor and before any VAO creation. The buffers are initially empty.
    void makeVBOs(GLFuncsQt_t& glfuncs);

    // Called only by the ctor, constructs the colored primitive drawing vertex array object.
    void makeColoredVAO(GLFuncsQt_t& glfuncs);

    // Called only by the ctor, constructs the colored and textured primitive drawing vertex
    // array object.
    void makeTexturedVAO(GLFuncsQt_t& glfuncs);

    // Places the contents of vertices in the vertices vbo.
    void uploadVertices(const std::vector<float>& vertices);

    // Places the contents of colors in the colors vbo.
    void uploadColors(const std::vector<std::uint8_t>& colors);

    // Places the contents of texcoords in the texCoords vbo.
    void uploadTexCoords(const std::vector<float>& texcoords);

    // Returns the currently set blend state variables that the renderer is interested in.
    BlendStates getBlendStates() const;

    // Returns the currently set line state variables that the renderer is interested in.
    LineStates getLineStates() const;

    // Sets the OpenGL blend state to match states.
    void setBlendStates(const BlendStates& states);

    // Sets the OpenGL line state to match states. Some line states may be impossible
    // and this call can generate an OpenGL error.
    void setLineStates(const LineStates& states);

    QSharedPointer<QOpenGLTexture> getTexture(const QString& name);

    std::unique_ptr<GLFuncsQt_t> m_glfuncs;
    std::unique_ptr<QOpenGLShaderProgram> m_colorProgram;
    std::unique_ptr<QOpenGLShaderProgram> m_textureProgram;
    std::map<QString, QSharedPointer<QOpenGLTexture>> m_namedTextures;
    QSharedPointer<QOpenGLTexture> m_defaultTexture;
    GLuint m_coloredVAO;
    GLuint m_texturedVAO;
    GLuint m_verticesVBO;
    GLuint m_coloursVBO;
    GLuint m_texCoordsVBO;
};

namespace
{

// Constructs and initialises the functions - throws if the functions
// could not be safely constructed and initialised.
std::unique_ptr<GLFuncsQt_t> makeOpenGLFunctions()
{
    std::unique_ptr<GLFuncsQt_t> funcs(new GLFuncsQt_t());
    if (!funcs->initializeOpenGLFunctions()) {
        throw std::runtime_error("initializeOpenGLFunctions failed.");
    }
    return std::move(funcs);
}

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

// Compiles but does not link the vertex shader source vs and the fragment shader source fs into
// an OpenGL program, and returns it as a pointer.
std::unique_ptr<QOpenGLShaderProgram> makeProgram(const char* vs, const char* fs)
{
    std::unique_ptr<QOpenGLShaderProgram> program(new QOpenGLShaderProgram());

    const bool vsOK = program->addShaderFromSourceCode(QOpenGLShader::Vertex, vs);
    const bool fsOK = program->addShaderFromSourceCode(QOpenGLShader::Fragment, fs);

    if (!(vsOK && fsOK)) {
        qDebug() << "Error creating QOpenGLShaderProgram.";
        throw std::runtime_error("Error creating QOpenGLShaderProgram.");
    }

    return std::move(program);
}

// Returns a compiled and linked program that draws colored and transformed primitives.
std::unique_ptr<QOpenGLShaderProgram> makeColorProgram(GLFuncsQt_t& glfuncs)
{
    ASSERT_OPENGL_OK;

    const char* vertexShader = "#version 330              \n"
                               "uniform mat4 transform;   \n"
                               "in vec2 point;            \n"
                               "in vec4 color;            \n"
                               "smooth out vec4 col;      \n"
                               "void main()               \n"
                               "{                         \n"
                               "   col.bgra = color;      \n"
                               "   gl_Position = transform * vec4(point, 0, 1); \n"
                               "}";

    const char* fragmentShader = "#version 330           \n"
                                 "in vec4 col;            \n"
                                 "out vec4 fragColor;     \n"
                                 "void main()            \n"
                                 "{                      \n"
                                 "   fragColor = col;    \n"
                                 "}";

    auto program = makeProgram(vertexShader, fragmentShader);

    program->bindAttributeLocation("point", VA_POINTS);
    program->bindAttributeLocation("color", VA_COLORS);

    glfuncs.glBindFragDataLocation(program->programId(), 0, "fragColor");

    program->link();

    ASSERT_OPENGL_OK;

    return std::move(program);
}

// Returns a compiled and linked program that draws colored, textured and transformed primitives.
std::unique_ptr<QOpenGLShaderProgram> makeTexProgram(GLFuncsQt_t& glfuncs)
{
    ASSERT_OPENGL_OK;

    const char* vertexShader = "#version 330                                      \n"
                               "uniform mat4 transform;                           \n"
                               "in vec2 point;                                    \n"
                               "in vec2 uv;                                       \n"
                               "in vec4 color;                                    \n"
                               "smooth out vec4 col;                              \n"
                               "smooth out vec2 texCoord;                         \n"
                               "void main()                                       \n"
                               "{                                                 \n"
                               "   texCoord = uv;                                 \n"
                               "   col.bgra = color;                              \n"
                               "   gl_Position = transform * vec4(point, 0, 1);   \n"
                               "}";

    const char* fragmentShader = "#version 330                                    \n"
                                 "uniform sampler2D tex;                          \n"
                                 "in vec2 texCoord;                               \n"
                                 "in vec4 col;                                    \n"
                                 "out vec4 fragColor;                             \n"
                                 "void main()                                     \n"
                                 "{                                               \n"
                                 "   vec4 texColor = texture(tex,texCoord);       \n"
                                 "   fragColor = col * texColor;                  \n"
                                 "}";

    auto program = makeProgram(vertexShader, fragmentShader);

    program->bindAttributeLocation("point", VA_POINTS);
    program->bindAttributeLocation("color", VA_COLORS);
    program->bindAttributeLocation("uv", VA_UVS);

    glfuncs.glBindFragDataLocation(program->programId(), 0, "fragColor");

    program->link();

    ASSERT_OPENGL_OK;

    return std::move(program);
}
}

Renderer::Internals::Internals(std::unique_ptr<GLFuncsQt_t> glfuncs)
    : m_glfuncs()
    , m_colorProgram(makeColorProgram(*glfuncs))
    , m_textureProgram(makeTexProgram(*glfuncs))
    , m_namedTextures()
    , m_defaultTexture(makeDefaultTexture())
    , m_coloredVAO(0)
    , m_texturedVAO(0)
{
    makeVBOs(*glfuncs);
    makeColoredVAO(*glfuncs);
    makeTexturedVAO(*glfuncs);

    m_glfuncs = std::move(glfuncs);

    ASSERT_OPENGL_OK;
}

Renderer::Internals::~Internals()
{
    m_namedTextures.clear();
    m_defaultTexture.clear();
}

void Renderer::Internals::makeVBOs(GLFuncsQt_t& glfuncs)
{
    ASSERT_OPENGL_OK;

    Q_ASSERT(m_verticesVBO != 0);
    Q_ASSERT(m_coloursVBO != 0);
    Q_ASSERT(m_texCoordsVBO != 0);

    glfuncs.glGenBuffers(1, &m_verticesVBO);
    glfuncs.glGenBuffers(1, &m_coloursVBO);
    glfuncs.glGenBuffers(1, &m_texCoordsVBO);

    glfuncs.glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::Internals::makeColoredVAO(GLFuncsQt_t& glfuncs)
{
    ASSERT_OPENGL_OK;

    Q_ASSERT(m_coloredVAO == 0);

    Q_ASSERT(m_verticesVBO != 0);
    Q_ASSERT(m_coloursVBO != 0);

    if (m_verticesVBO == 0 || m_coloursVBO == 0) {
        throw std::logic_error("makeColoredVAO called before construction of VBOs.");
    }

    glfuncs.glGenVertexArrays(1, &m_coloredVAO);
    glfuncs.glBindVertexArray(m_coloredVAO);

    glfuncs.glBindBuffer(GL_ARRAY_BUFFER, m_verticesVBO);
    glfuncs.glEnableVertexAttribArray(VA_POINTS);
    glfuncs.glVertexAttribPointer(VA_POINTS, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glfuncs.glBindBuffer(GL_ARRAY_BUFFER, m_coloursVBO);
    glfuncs.glEnableVertexAttribArray(VA_COLORS);
    glfuncs.glVertexAttribPointer(VA_COLORS, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);

    glfuncs.glBindVertexArray(0);
    glfuncs.glBindBuffer(GL_ARRAY_BUFFER, 0);

    ASSERT_OPENGL_OK;
}

void Renderer::Internals::makeTexturedVAO(GLFuncsQt_t& glfuncs)
{
    ASSERT_OPENGL_OK;

    Q_ASSERT(m_texturedVAO == 0);

    Q_ASSERT(m_verticesVBO != 0);
    Q_ASSERT(m_coloursVBO != 0);
    Q_ASSERT(m_texCoordsVBO != 0);

    if (m_verticesVBO == 0 || m_coloursVBO == 0 || m_texCoordsVBO == 0) {
        throw std::logic_error("makeTexturedVAO called before construction of VBOs.");
    }

    glfuncs.glGenVertexArrays(1, &m_texturedVAO);
    glfuncs.glBindVertexArray(m_texturedVAO);

    glfuncs.glBindBuffer(GL_ARRAY_BUFFER, m_verticesVBO);
    glfuncs.glEnableVertexAttribArray(VA_POINTS);
    glfuncs.glVertexAttribPointer(VA_POINTS, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glfuncs.glBindBuffer(GL_ARRAY_BUFFER, m_coloursVBO);
    glfuncs.glEnableVertexAttribArray(VA_COLORS);
    glfuncs.glVertexAttribPointer(VA_COLORS, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);

    glfuncs.glBindBuffer(GL_ARRAY_BUFFER, m_texCoordsVBO);
    glfuncs.glEnableVertexAttribArray(VA_UVS);
    glfuncs.glVertexAttribPointer(VA_UVS, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glfuncs.glBindVertexArray(0);
    glfuncs.glBindBuffer(GL_ARRAY_BUFFER, 0);

    ASSERT_OPENGL_OK;
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

void Renderer::Internals::setBlendStates(const BlendStates& states)
{

    if (states.blendingEnabled) {
        m_glfuncs->glEnable(GL_BLEND);
    } else {
        m_glfuncs->glDisable(GL_BLEND);
    }

    m_glfuncs->glBlendFunc(states.blendSrc, states.blendDst);
}

Renderer::Internals::BlendStates Renderer::Internals::getBlendStates() const
{
    Renderer::Internals::BlendStates states;
    states.blendingEnabled = m_glfuncs->glIsEnabled(GL_BLEND);
    m_glfuncs->glGetIntegerv(GL_BLEND_SRC, &states.blendSrc);
    m_glfuncs->glGetIntegerv(GL_BLEND_DST, &states.blendDst);

    return states;
}

void Renderer::Internals::setLineStates(const LineStates& states)
{
    // This call can generate errors when the line width requested is
    // not available on the machine. Typically smoothed lines may only be
    // width 1.0.

    // Typical ranges of non smoothed lines are 0 to 10.

    ASSERT_OPENGL_OK;

    if (states.smoothingEnabled) {
        m_glfuncs->glEnable(GL_LINE_SMOOTH);
    } else {
        m_glfuncs->glDisable(GL_LINE_SMOOTH);
    }

    m_glfuncs->glLineWidth(states.lineWidth);

    ASSERT_OPENGL_OK;
}

Renderer::Internals::LineStates Renderer::Internals::getLineStates() const
{
    Renderer::Internals::LineStates states;
    states.smoothingEnabled = m_glfuncs->glIsEnabled(GL_LINE_SMOOTH);
    m_glfuncs->glGetFloatv(GL_LINE_WIDTH, &states.lineWidth);

    return states;
}

void Renderer::Internals::uploadVertices(const std::vector<float>& vertices)
{
    Q_ASSERT(m_verticesVBO != 0);
    m_glfuncs->glBindBuffer(GL_ARRAY_BUFFER, m_verticesVBO);
    m_glfuncs->glBufferData(GL_ARRAY_BUFFER,
                            vertices.size() * sizeof(float),
                            vertices.data(),
                            GL_STREAM_DRAW);
    m_glfuncs->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::Internals::uploadColors(const std::vector<std::uint8_t>& colors)
{
    Q_ASSERT(m_coloursVBO != 0);
    m_glfuncs->glBindBuffer(GL_ARRAY_BUFFER, m_coloursVBO);
    m_glfuncs->glBufferData(GL_ARRAY_BUFFER, colors.size(), colors.data(), GL_STREAM_DRAW);
    m_glfuncs->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::Internals::uploadTexCoords(const std::vector<float>& texcoords)
{
    Q_ASSERT(m_texCoordsVBO != 0);
    m_glfuncs->glBindBuffer(GL_ARRAY_BUFFER, m_texCoordsVBO);
    m_glfuncs->glBufferData(GL_ARRAY_BUFFER,
                            texcoords.size() * sizeof(float),
                            texcoords.data(),
                            GL_STREAM_DRAW);
    m_glfuncs->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::Internals::draw(const QMatrix4x4& transform, const ColoredLines& lines)
{
    // Find out what the current OpenGL context state is, so we can reset it afterwards.

    const auto originalLineStates = getLineStates();
    const auto originalBlendStates = getBlendStates();

    const BlendStates classicBlending(GL_TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // We only drawn smooth lines of width 1.
    const LineStates smoothLine(GL_TRUE, 1.0f);

    ASSERT_OPENGL_OK;
    if (originalLineStates != smoothLine) {
        setLineStates(smoothLine);
    }

    ASSERT_OPENGL_OK;
    if (classicBlending != originalBlendStates) {
        setBlendStates(classicBlending);
    }

    ASSERT_OPENGL_OK;
    uploadVertices(lines.m_lines);
    ASSERT_OPENGL_OK;
    uploadColors(lines.m_colors);

    ASSERT_OPENGL_OK;
    m_colorProgram->bind();
    m_colorProgram->setUniformValue("transform", transform);

    ASSERT_OPENGL_OK;
    m_glfuncs->glBindVertexArray(m_coloredVAO);
    m_glfuncs->glDrawArrays(GL_LINES, 0, 2u * static_cast<GLsizei>(lines.lineCount()));
    m_glfuncs->glBindVertexArray(0);

    ASSERT_OPENGL_OK;
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
    // Find out what the current OpenGL context state is, so we can reset it afterwards.

    const auto originalBlendStates = getBlendStates();

    const BlendStates classicBlending(GL_TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (classicBlending != originalBlendStates) {
        setBlendStates(classicBlending);
    }

    uploadVertices(quads.m_quads);
    uploadColors(quads.m_quadColors);

    m_colorProgram->bind();
    m_colorProgram->setUniformValue("transform", transform);

    m_glfuncs->glBindVertexArray(m_coloredVAO);
    m_glfuncs->glDrawArrays(GL_TRIANGLES, 0, 6u * static_cast<GLsizei>(quads.quadCount()));
    m_glfuncs->glBindVertexArray(0);
    m_colorProgram->release();

    // Reset the OpenGL context states.

    if (classicBlending != originalBlendStates) {
        setBlendStates(originalBlendStates);
    }

    ASSERT_OPENGL_OK;
}

void Renderer::Internals::draw(const QMatrix4x4& transform,
                               const QString& textureName,
                               const TexturedQuads& quads)
{
    ASSERT_OPENGL_OK;

    auto texture = getTexture(textureName);

    // Find out what the current OpenGL context state is, so we can reset it afterwards.

    const auto originalBlendStates = getBlendStates();

    const BlendStates classicBlending(GL_TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (classicBlending != originalBlendStates) {
        setBlendStates(classicBlending);
    }

    ASSERT_OPENGL_OK;

    uploadVertices(quads.m_quads.m_quads);
    uploadColors(quads.m_quads.m_quadColors);
    uploadTexCoords(quads.m_textureCoords);

    ASSERT_OPENGL_OK;

    m_textureProgram->bind();
    m_textureProgram->setUniformValue("transform", transform);
    m_textureProgram->setUniformValue("tex", 0);

    texture->bind(0);
    
    ASSERT_OPENGL_OK;

    m_glfuncs->glBindVertexArray(m_texturedVAO);
    m_glfuncs->glDrawArrays(GL_TRIANGLES, 0, 6u * static_cast<GLsizei>(quads.quadCount()));
    m_glfuncs->glBindBuffer(GL_ARRAY_BUFFER, 0);

    ASSERT_OPENGL_OK;

    texture->release(0);
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
    : m_internals(std::make_shared<Renderer::Internals>(makeOpenGLFunctions()))
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
                    const TexturedQuads& quads)
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
