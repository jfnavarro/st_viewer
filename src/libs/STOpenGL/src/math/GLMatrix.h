/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLMATRIX_H
#define GLMATRIX_H

#include "GLCommon.h"

namespace GL
{
// Simple implementation of linear algebraical matrix. Used primarily as
// a means of easily manipulating the opengl matrices.
//TODO extend if needed.
class GLmatrix
{
public:

    inline GLmatrix();
    inline explicit GLmatrix(GL::Initialization);
    inline GLmatrix(GLfloat m11, GLfloat m12, GLfloat m13,
                    GLfloat m21, GLfloat m22, GLfloat m23,
                    GLfloat m31, GLfloat m32, GLfloat m33);

    inline const GLfloat* data() const;
    inline GLfloat* data();

    //NOTE type cast operators (remove to avoid implicit cast?)
    inline operator const GLfloat*() const;
    inline operator GLfloat*();

    // access data by standard m<row><column>-notation
    enum {
        M11 = 0,
        M12 = 1,
        M13 = 2,
        M14 = 3,
        M21 = 4,
        M22 = 5,
        M23 = 6,
        M24 = 7,
        M31 = 8,
        M32 = 9,
        M33 = 10,
        M34 = 11,
        M41 = 12,
        M42 = 13,
        M43 = 14,
        M44 = 15
    };
    GLfloat m[16];
};

inline const bool fuzzyEqual(const GLmatrix& m0, const GLmatrix& m1, GLfloat e = EPSILON);
inline const bool fuzzyNotEqual(const GLmatrix& m0, const GLmatrix& m1, GLfloat e = EPSILON);

inline const bool operator ==(const GLmatrix &m0, const GLmatrix &m1);
inline const bool operator !=(const GLmatrix &m0, const GLmatrix &m1);

} // namespace GL //

/****************************************** DEFINITION ******************************************/

namespace GL
{
GLmatrix::GLmatrix()
{
    m[M11] = 1.0f;
    m[M12] = 0.0f;
    m[M13] = 0.0f;
    m[M14] = 0.0f;
    m[M21] = 0.0f;
    m[M22] = 1.0f;
    m[M23] = 0.0f;
    m[M24] = 0.0f;
    m[M31] = 0.0f;
    m[M32] = 0.0f;
    m[M33] = 1.0f;
    m[M34] = 0.0f;
    m[M41] = 0.0f;
    m[M42] = 0.0f;
    m[M43] = 0.0f;
    m[M44] = 1.0f;
}
GLmatrix::GLmatrix(GL::Initialization) { }
GLmatrix::GLmatrix(GLfloat m11, GLfloat m12, GLfloat m13,
                   GLfloat m21, GLfloat m22, GLfloat m23,
                   GLfloat m31, GLfloat m32, GLfloat m33)
{
    m[M11] = m11;
    m[M12] = m12;
    m[M13] = 0.0f;
    m[M14] = m13;
    m[M21] = m21;
    m[M22] = m22;
    m[M23] = 0.0f;
    m[M24] = m23;
    m[M31] = 0.0f;
    m[M32] = 0.0f;
    m[M33] = 1.0f;
    m[M34] = 0.0f;
    m[M41] = m31;
    m[M42] = m32;
    m[M43] = 0.0f;
    m[M44] = m33;
}

const GLfloat* GLmatrix::data() const
{
    return &(m[0]);
}
GLfloat* GLmatrix::data()
{
    return &(m[0]);
}

GLmatrix::operator const GLfloat*() const
{
    return &(m[0]);
}
GLmatrix::operator GLfloat*()
{
    return &(m[0]);
}

const bool fuzzyEqual(const GLmatrix& m0, const GLmatrix& m1, GLfloat e)
{
    return fuzzyEqual(m0.m[GLmatrix::M11], m1.m[GLmatrix::M11], e)
           && fuzzyEqual(m0.m[GLmatrix::M12], m1.m[GLmatrix::M12], e)
           && fuzzyEqual(m0.m[GLmatrix::M13], m1.m[GLmatrix::M13], e)
           && fuzzyEqual(m0.m[GLmatrix::M14], m1.m[GLmatrix::M14], e)
           && fuzzyEqual(m0.m[GLmatrix::M21], m1.m[GLmatrix::M21], e)
           && fuzzyEqual(m0.m[GLmatrix::M22], m1.m[GLmatrix::M22], e)
           && fuzzyEqual(m0.m[GLmatrix::M23], m1.m[GLmatrix::M23], e)
           && fuzzyEqual(m0.m[GLmatrix::M24], m1.m[GLmatrix::M24], e)
           && fuzzyEqual(m0.m[GLmatrix::M31], m1.m[GLmatrix::M31], e)
           && fuzzyEqual(m0.m[GLmatrix::M32], m1.m[GLmatrix::M32], e)
           && fuzzyEqual(m0.m[GLmatrix::M33], m1.m[GLmatrix::M33], e)
           && fuzzyEqual(m0.m[GLmatrix::M34], m1.m[GLmatrix::M34], e)
           && fuzzyEqual(m0.m[GLmatrix::M41], m1.m[GLmatrix::M41], e)
           && fuzzyEqual(m0.m[GLmatrix::M42], m1.m[GLmatrix::M42], e)
           && fuzzyEqual(m0.m[GLmatrix::M43], m1.m[GLmatrix::M43], e)
           && fuzzyEqual(m0.m[GLmatrix::M44], m1.m[GLmatrix::M44], e);
}
const bool fuzzyNotEqual(const GLmatrix& m0, const GLmatrix& m1, GLfloat e)
{
    return fuzzyNotEqual(m0.m[GLmatrix::M11], m1.m[GLmatrix::M11], e)
           || fuzzyNotEqual(m0.m[GLmatrix::M12], m1.m[GLmatrix::M12], e)
           || fuzzyNotEqual(m0.m[GLmatrix::M13], m1.m[GLmatrix::M13], e)
           || fuzzyNotEqual(m0.m[GLmatrix::M14], m1.m[GLmatrix::M14], e)
           || fuzzyNotEqual(m0.m[GLmatrix::M21], m1.m[GLmatrix::M21], e)
           || fuzzyNotEqual(m0.m[GLmatrix::M22], m1.m[GLmatrix::M22], e)
           || fuzzyNotEqual(m0.m[GLmatrix::M23], m1.m[GLmatrix::M23], e)
           || fuzzyNotEqual(m0.m[GLmatrix::M24], m1.m[GLmatrix::M24], e)
           || fuzzyNotEqual(m0.m[GLmatrix::M31], m1.m[GLmatrix::M31], e)
           || fuzzyNotEqual(m0.m[GLmatrix::M32], m1.m[GLmatrix::M32], e)
           || fuzzyNotEqual(m0.m[GLmatrix::M33], m1.m[GLmatrix::M33], e)
           || fuzzyNotEqual(m0.m[GLmatrix::M34], m1.m[GLmatrix::M34], e)
           || fuzzyNotEqual(m0.m[GLmatrix::M41], m1.m[GLmatrix::M41], e)
           || fuzzyNotEqual(m0.m[GLmatrix::M42], m1.m[GLmatrix::M42], e)
           || fuzzyNotEqual(m0.m[GLmatrix::M43], m1.m[GLmatrix::M43], e)
           || fuzzyNotEqual(m0.m[GLmatrix::M44], m1.m[GLmatrix::M44], e);
}

const bool operator ==(const GLmatrix &m0, const GLmatrix &m1)
{
    return m0.m[GLmatrix::M11] == m1.m[GLmatrix::M11]
           && m0.m[GLmatrix::M12] == m1.m[GLmatrix::M12]
           && m0.m[GLmatrix::M13] == m1.m[GLmatrix::M13]
           && m0.m[GLmatrix::M14] == m1.m[GLmatrix::M14]
           && m0.m[GLmatrix::M21] == m1.m[GLmatrix::M21]
           && m0.m[GLmatrix::M22] == m1.m[GLmatrix::M22]
           && m0.m[GLmatrix::M23] == m1.m[GLmatrix::M23]
           && m0.m[GLmatrix::M24] == m1.m[GLmatrix::M24]
           && m0.m[GLmatrix::M31] == m1.m[GLmatrix::M31]
           && m0.m[GLmatrix::M32] == m1.m[GLmatrix::M32]
           && m0.m[GLmatrix::M33] == m1.m[GLmatrix::M33]
           && m0.m[GLmatrix::M34] == m1.m[GLmatrix::M34]
           && m0.m[GLmatrix::M41] == m1.m[GLmatrix::M41]
           && m0.m[GLmatrix::M42] == m1.m[GLmatrix::M42]
           && m0.m[GLmatrix::M43] == m1.m[GLmatrix::M43]
           && m0.m[GLmatrix::M44] == m1.m[GLmatrix::M44];
}
const bool operator !=(const GLmatrix &m0, const GLmatrix &m1)
{
    return m0.m[GLmatrix::M11] != m1.m[GLmatrix::M11]
           || m0.m[GLmatrix::M12] != m1.m[GLmatrix::M12]
           || m0.m[GLmatrix::M13] != m1.m[GLmatrix::M13]
           || m0.m[GLmatrix::M14] != m1.m[GLmatrix::M14]
           || m0.m[GLmatrix::M21] != m1.m[GLmatrix::M21]
           || m0.m[GLmatrix::M22] != m1.m[GLmatrix::M22]
           || m0.m[GLmatrix::M23] != m1.m[GLmatrix::M23]
           || m0.m[GLmatrix::M24] != m1.m[GLmatrix::M24]
           || m0.m[GLmatrix::M31] != m1.m[GLmatrix::M31]
           || m0.m[GLmatrix::M32] != m1.m[GLmatrix::M32]
           || m0.m[GLmatrix::M33] != m1.m[GLmatrix::M33]
           || m0.m[GLmatrix::M34] != m1.m[GLmatrix::M34]
           || m0.m[GLmatrix::M41] != m1.m[GLmatrix::M41]
           || m0.m[GLmatrix::M42] != m1.m[GLmatrix::M42]
           || m0.m[GLmatrix::M43] != m1.m[GLmatrix::M43]
           || m0.m[GLmatrix::M44] != m1.m[GLmatrix::M44];
}

} // namespace GL //

#endif // GLMATRIX_H //
