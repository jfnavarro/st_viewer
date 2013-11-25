/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLCOLOR_H
#define GLCOLOR_H

#include "GLCommon.h"

namespace GL
{

// color data structures

// simple colors
enum GlobalColor {
    Black = 0,
    White,
    MediumGray,
    LightGray,
    Red,
    Green,
    Blue,
    Cyan,
    Magenta,
    Yellow,
    DarkRed,
    DarkGreen,
    DarkBlue,
    DarkCyan,
    DarkMagenta,
    DarkYellow,
    Transparent
};

// 32bit internal color type
typedef GLuint GLrgb;

inline GLubyte alpha(const GLrgb argb);
inline GLubyte red(const GLrgb argb);
inline GLubyte green(const GLrgb argb);
inline GLubyte blue(const GLrgb argb);

// opengl compliant normalized colors [0.0, 1.0] (float)
struct GLcolor {
    inline GLcolor();
    inline explicit GLcolor(GL::Initialization);
    explicit GLcolor(GL::GlobalColor color);
    inline GLcolor(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha = 0xFFu);
    inline GLcolor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha = 1.0f);
    inline void setColor(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha = 0xFFu);
    inline void setColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha = 1.0f);

    GLfloat red;
    GLfloat green;
    GLfloat blue;
    GLfloat alpha;
};

inline const bool fuzzyEqual(const GLcolor &c0, const GLcolor &c1, GLfloat e = EPSILON);
inline const bool fuzzyNotEqual(const GLcolor &c0, const GLcolor &c1, GLfloat e = EPSILON);

// linear interpolation
inline const GLcolor lerp(const GLfloat t, const GLcolor &c0, const GLcolor &c1);
// inverse linear interpolation
inline const GLcolor invlerp(const GLfloat t, const GLcolor &c0, const GLcolor &c1);

inline const GLcolor operator *(GLfloat blend, const GLcolor &color);
inline const GLcolor operator *(const GLcolor &color, GLfloat blend);

template <int N>
struct GLcolordata {
    static const GLuint COLORS = N;

    inline GLcolordata();
    inline explicit GLcolordata(GL::Initialization);
    inline explicit GLcolordata(const GLcolor &color);
    inline void setColor(const GLcolor &color, const GLindex index);
    inline void setColors(const GLcolor &color);

    GLcolor c[N];
};

typedef GLcolordata<2> GLlinecolor;
typedef GLcolordata<3> GLtrianglecolor;
typedef GLcolordata<4> GLrectanglecolor;

} // namespace GL //

/****************************************** DEFINITION ******************************************/

namespace GL
{

// color data structures

// simple colors
inline GLubyte alpha(const GLrgb argb)
{
    return ((argb >> 24) & 0xFFu);
}

inline GLubyte red(const GLrgb argb)
{
    return ((argb >> 16) & 0xFFu);
}

inline GLubyte green(const GLrgb argb)
{
    return ((argb >> 8) & 0xFFu);
}

inline GLubyte blue(const GLrgb argb)
{
    return ((argb >> 0) & 0xFFu);
}

// normalized colors
inline GLcolor::GLcolor() : red(1.0f), green(1.0f), blue(1.0f), alpha(1.0f) { }
inline GLcolor::GLcolor(GL::Initialization) { }
inline GLcolor::GLcolor(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
    : red((GLfloat) red / (GLfloat) 0xFFu),
      green((GLfloat) green / (GLfloat) 0xFFu),
      blue((GLfloat) blue / (GLfloat) 0xFFu),
      alpha((GLfloat) alpha / (GLfloat) 0xFFu)
{

}

inline GLcolor::GLcolor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
    : red(red), green(green), blue(blue), alpha(alpha)
{

}

inline void GLcolor::setColor(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
    GLcolor::red = (GLfloat) red / (GLfloat) 0xFFu;
    GLcolor::green = (GLfloat) green / (GLfloat) 0xFFu;
    GLcolor::blue = (GLfloat) blue / (GLfloat) 0xFFu;
    GLcolor::alpha = (GLfloat) alpha / (GLfloat) 0xFFu;
}
inline void GLcolor::setColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    GLcolor::red = red;
    GLcolor::green = green;
    GLcolor::blue = blue;
    GLcolor::alpha = alpha;
}

inline const bool fuzzyEqual(const GLcolor &c0, const GLcolor &c1, GLfloat e)
{
    return fuzzyEqual(c0.red, c1.red, e)
           && fuzzyEqual(c0.green, c0.green, e)
           && fuzzyEqual(c0.blue, c1.blue, e)
           && fuzzyEqual(c0.alpha, c1.alpha, e);
}
inline const bool fuzzyNotEqual(const GLcolor &c0, const GLcolor &c1, GLfloat e)
{
    return fuzzyNotEqual(c0.red, c1.red, e)
           || fuzzyNotEqual(c0.green, c0.green, e)
           || fuzzyNotEqual(c0.blue, c1.blue, e)
           || fuzzyNotEqual(c0.alpha, c1.alpha, e);
}

inline const GLcolor lerp(const GLfloat t, const GLcolor &c0, const GLcolor &c1)
{
    return GLcolor(
               (c0.red + (c1.red - c0.red) * t),
               (c0.green + (c1.green - c0.green) * t),
               (c0.blue + (c1.blue - c0.blue) * t),
               (c0.alpha + (c1.alpha - c0.alpha) * t)
           );
}

inline const GLcolor invlerp(const GLfloat t, const GLcolor &c0, const GLcolor &c1)
{
    const GLfloat invt = 1.0f / (1.0f - t);
    return GLcolor(
               (c0.red - t * c1.red) * invt,
               (c0.green - t * c1.green) * invt,
               (c0.blue - t * c1.blue) * invt,
               (c0.alpha - t * c1.alpha) * invt
           );
}

inline const GLcolor operator *(GLfloat blend, const GLcolor &color)
{
    return GLcolor(color.red, color.green, color.blue, blend * color.alpha);
}
inline const GLcolor operator *(const GLcolor &color, GLfloat blend)
{
    return GLcolor(color.red, color.green, color.blue, blend * color.alpha);
}

template <int N>
inline GLcolordata<N>::GLcolordata()
{
    setColors(GLcolor());
}

template <int N>
inline GLcolordata<N>::GLcolordata(GL::Initialization) { }
template <int N>
inline GLcolordata<N>::GLcolordata(const GLcolor &color)
{
    setColors(color);
}

template <int N>
inline void GLcolordata<N>::setColor(const GLcolor &color, const GLindex index)
{
    Q_ASSERT(index < GLcolordata<N>::COLORS);
    c[index] = color;
}
template <int N>
inline void GLcolordata<N>::setColors(const GLcolor &color)
{
    for (GLuint i = 0; i < GLcolordata<N>::COLORS; ++i) {
        c[i] = color;
    }
}

} // namespace GL //

#endif // GLCOLOR_H //
