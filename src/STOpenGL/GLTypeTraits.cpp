/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GLTypeTraits.h"

namespace GL
{
// opengl type trait
template <> const GLenum GLTypeTrait<GLbyte>::type_enum = GL_BYTE;
template <> const GLenum GLTypeTrait<GLubyte>::type_enum = GL_UNSIGNED_BYTE;
template <> const GLenum GLTypeTrait<GLshort>::type_enum = GL_SHORT;
template <> const GLenum GLTypeTrait<GLushort>::type_enum = GL_UNSIGNED_SHORT;
template <> const GLenum GLTypeTrait<GLint>::type_enum = GL_INT;
template <> const GLenum GLTypeTrait<GLuint>::type_enum = GL_UNSIGNED_INT;
template <> const GLenum GLTypeTrait<GLfloat>::type_enum = GL_FLOAT;
template <> const GLenum GLTypeTrait<GLdouble>::type_enum = GL_DOUBLE;

template <> const GLbyte GLTypeTrait<GLbyte>::min_value = GLbyte(0x80);
template <> const GLubyte GLTypeTrait<GLubyte>::min_value = GLubyte(0x00);
template <> const GLshort GLTypeTrait<GLshort>::min_value = GLshort(0x8000);
template <> const GLushort GLTypeTrait<GLushort>::min_value = GLushort(0x0000);
template <> const GLint GLTypeTrait<GLint>::min_value = GLint(0x80000000);
template <> const GLuint GLTypeTrait<GLuint>::min_value = GLuint(0x00000000);
template <> const GLfloat GLTypeTrait<GLfloat>::min_value = FLT_MIN;
template <> const GLdouble GLTypeTrait<GLdouble>::min_value = DBL_MIN;

template <> const GLbyte GLTypeTrait<GLbyte>::max_value = GLbyte(0x7F);
template <> const GLubyte GLTypeTrait<GLubyte>::max_value = GLubyte(0xFF);
template <> const GLshort GLTypeTrait<GLshort>::max_value = GLshort(0x7FFF);
template <> const GLushort GLTypeTrait<GLushort>::max_value = GLushort(0xFFFF);
template <> const GLint GLTypeTrait<GLint>::max_value = GLint(0x7FFFFFFF);
template <> const GLuint GLTypeTrait<GLuint>::max_value = GLuint(0xFFFFFFFF);
template <> const GLfloat GLTypeTrait<GLfloat>::max_value = FLT_MAX;
template <> const GLdouble GLTypeTrait<GLdouble>::max_value = DBL_MAX;

// opengl type enum trait
template <> const int GLTypeEnumTrait<GL_BYTE>::type_size = sizeof(GLbyte);             // GL_BYTE           0x1400
template <> const int GLTypeEnumTrait<GL_UNSIGNED_BYTE>::type_size = sizeof(GLubyte);   // GL_UNSIGNED_BYTE  0x1401
template <> const int GLTypeEnumTrait<GL_SHORT>::type_size = sizeof(GLshort);           // GL_SHORT          0x1402
template <> const int GLTypeEnumTrait<GL_UNSIGNED_SHORT>::type_size = sizeof(GLushort); // GL_UNSIGNED_SHORT 0x1403
template <> const int GLTypeEnumTrait<GL_INT>::type_size = sizeof(GLint);               // GL_INT            0x1404
template <> const int GLTypeEnumTrait<GL_UNSIGNED_INT>::type_size = sizeof(GLuint);     // GL_UNSIGNED_INT   0x1405
template <> const int GLTypeEnumTrait<GL_FLOAT>::type_size = sizeof(GLfloat);           // GL_FLOAT          0x1406
template <> const int GLTypeEnumTrait<GL_2_BYTES>::type_size = (2 * sizeof(GLbyte));    // GL_2_BYTES        0x1407
template <> const int GLTypeEnumTrait<GL_3_BYTES>::type_size = (3 * sizeof(GLbyte));    // GL_3_BYTES        0x1408
template <> const int GLTypeEnumTrait<GL_4_BYTES>::type_size = (4 * sizeof(GLbyte));    // GL_4_BYTES        0x1409
template <> const int GLTypeEnumTrait<GL_DOUBLE>::type_size = sizeof(GLdouble);         // GL_DOUBLE         0x140A

// opengl type enum info
int GLTypeEnumInfo::type_size(const GLenum type)
{
    static const int map[] = {
        GLTypeEnumTrait<GL_BYTE>::type_size,
        GLTypeEnumTrait<GL_UNSIGNED_BYTE>::type_size,
        GLTypeEnumTrait<GL_SHORT>::type_size,
        GLTypeEnumTrait<GL_UNSIGNED_SHORT>::type_size,
        GLTypeEnumTrait<GL_INT>::type_size,
        GLTypeEnumTrait<GL_UNSIGNED_INT>::type_size,
        GLTypeEnumTrait<GL_FLOAT>::type_size,
        GLTypeEnumTrait<GL_2_BYTES>::type_size,
        GLTypeEnumTrait<GL_3_BYTES>::type_size,
        GLTypeEnumTrait<GL_4_BYTES>::type_size,
        GLTypeEnumTrait<GL_DOUBLE>::type_size
    };
    return map[type - GL_BYTE];
}

// opengl color enum traits
template <> const int GLColorEnumTrait<GL_COLOR_INDEX>::color_count = 1;      // GL_COLOR_INDEX     0x1900
template <> const int GLColorEnumTrait<GL_STENCIL_INDEX>::color_count = 1;    // GL_STENCIL_INDEX   0x1901
template <> const int GLColorEnumTrait<GL_DEPTH_COMPONENT>::color_count = 1;  // GL_DEPTH_COMPONENT 0x1902
template <> const int GLColorEnumTrait<GL_RED>::color_count = 1;              // GL_RED             0x1903
template <> const int GLColorEnumTrait<GL_GREEN>::color_count = 1;            // GL_GREEN           0x1904
template <> const int GLColorEnumTrait<GL_BLUE>::color_count = 1;             // GL_BLUE            0x1905
template <> const int GLColorEnumTrait<GL_ALPHA>::color_count = 1;            // GL_ALPHA           0x1906
template <> const int GLColorEnumTrait<GL_RGB>::color_count = 3;              // GL_RGB             0x1907
template <> const int GLColorEnumTrait<GL_RGBA>::color_count = 4;             // GL_RGBA            0x1908
template <> const int GLColorEnumTrait<GL_LUMINANCE>::color_count = 1;        // GL_LUMINANCE       0x1909
template <> const int GLColorEnumTrait<GL_LUMINANCE_ALPHA>::color_count = 2;  // GL_LUMINANCE_ALPHA 0x190A

// opengl color enum info
int GLColorEnumInfo::color_count(const GLenum mode)
{
    static const int map[] = {
        GLColorEnumTrait<GL_COLOR_INDEX>::color_count,
        GLColorEnumTrait<GL_STENCIL_INDEX>::color_count,
        GLColorEnumTrait<GL_DEPTH_COMPONENT>::color_count,
        GLColorEnumTrait<GL_RED>::color_count,
        GLColorEnumTrait<GL_GREEN>::color_count,
        GLColorEnumTrait<GL_BLUE>::color_count,
        GLColorEnumTrait<GL_ALPHA>::color_count,
        GLColorEnumTrait<GL_RGB>::color_count,
        GLColorEnumTrait<GL_RGBA>::color_count,
        GLColorEnumTrait<GL_LUMINANCE>::color_count,
        GLColorEnumTrait<GL_LUMINANCE_ALPHA>::color_count
    };
    return map[mode - GL_COLOR_INDEX];
}

// opengl geometric type trait
template <> const int GLGeomEnumTrait<GL_POINTS>::vertex_count = 1;          // GL_POINTS         0x0000
template <> const int GLGeomEnumTrait<GL_LINES>::vertex_count = 2;           // GL_LINES          0x0001
template <> const int GLGeomEnumTrait<GL_LINE_LOOP>::vertex_count = -1;      // GL_LINE_LOOP      0x0002
template <> const int GLGeomEnumTrait<GL_LINE_STRIP>::vertex_count = -1;     // GL_LINE_STRIP     0x0003
template <> const int GLGeomEnumTrait<GL_TRIANGLES>::vertex_count = 3;       // GL_TRIANGLES      0x0004
template <> const int GLGeomEnumTrait<GL_TRIANGLE_STRIP>::vertex_count = -1; // GL_TRIANGLE_STRIP 0x0005
template <> const int GLGeomEnumTrait<GL_TRIANGLE_FAN>::vertex_count = -1;   // GL_TRIANGLE_FAN   0x0006
template <> const int GLGeomEnumTrait<GL_QUADS>::vertex_count = 4;           // GL_QUADS          0x0007
template <> const int GLGeomEnumTrait<GL_QUAD_STRIP>::vertex_count = -1;     // GL_QUAD_STRIP     0x0008
template <> const int GLGeomEnumTrait<GL_POLYGON>::vertex_count = -1;        // GL_POLYGON        0x0009

// opengl geometric enum info
int GLGeomEnumInfo::vertex_count(const GLenum mode)
{
    static const int map[] = {
        GLGeomEnumTrait<GL_POINTS>::vertex_count,
        GLGeomEnumTrait<GL_LINES>::vertex_count,
        GLGeomEnumTrait<GL_LINE_LOOP>::vertex_count,
        GLGeomEnumTrait<GL_LINE_STRIP>::vertex_count,
        GLGeomEnumTrait<GL_TRIANGLES>::vertex_count,
        GLGeomEnumTrait<GL_TRIANGLE_STRIP>::vertex_count,
        GLGeomEnumTrait<GL_TRIANGLE_FAN>::vertex_count,
        GLGeomEnumTrait<GL_QUADS>::vertex_count,
        GLGeomEnumTrait<GL_QUAD_STRIP>::vertex_count,
        GLGeomEnumTrait<GL_POLYGON>::vertex_count,
    };
    return map[mode - GL_POINTS];
}

} // namespace GL //
