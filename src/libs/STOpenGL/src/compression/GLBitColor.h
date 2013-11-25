/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLBITCOLOR_H
#define GLBITCOLOR_H

#include "GLCommon.h"
#include "GLColor.h"

namespace GL
{
// the following name space functions implement color compression
// functionality. Color is a data heavy aspect of rendering and in
// case the default 128bit color space is not needed the following
// function provides a means of compressing to opengl format
// compressed colors.

// define compressed color types
typedef GLubyte GLcolorRGB8b;
typedef GLushort GLcolorRGB16b;

// compress color to a standard [3,3,2] bit allocation of RGB color space (alpha excluded)
inline const GLcolorRGB8b compressToRGB8Bit(const GLcolor &color);
// compress color to a standard [5,6,5] bit allocation of RGB color space (alpha excluded)
inline const GLcolorRGB16b compressToRGB16Bit(const GLcolor &color);

inline const GLcolor decompressFromRGB8Bit(const GLcolorRGB8b &color);
inline const GLcolor decompressFromRGB16Bit(const GLcolorRGB16b &color);

} // namespace GL //

/****************************************** DEFINITION ******************************************/

namespace GL
{

inline const GLcolorRGB8b compressToRGB8Bit(const GLcolor &color)
{
    return
        ((GLubyte(color.red * 7.0f + 0.5f) & 0x07u) << 5) |
        ((GLubyte(color.green * 7.0f + 0.5f) & 0x07u) << 2) |
        ((GLubyte(color.blue * 3.0f + 0.5f) & 0x03u) << 0);

}

inline const GLcolorRGB16b compressToRGB16Bit(const GLcolor &color)
{
    return
        ((GLushort(color.red * 31.0f + 0.5f) & 0x001Fu) << 11) |
        ((GLushort(color.green * 63.0f + 0.5f) & 0x003Fu) << 5) |
        ((GLushort(color.blue * 31.0f + 0.5f) & 0x001Fu) << 0);

}

inline const GLcolor decompressFromRGB8Bit(const GLcolorRGB8b &color)
{
    const GLfloat c_inv_7 = (1.0f / 7.0f);
    const GLfloat c_inv_3 = (1.0f / 3.0f);
    return GLcolor(
               float((color >> 5) & 0x07u) * c_inv_7,
               float((color >> 2) & 0x07u) * c_inv_7,
               float((color >> 0) & 0x03u) * c_inv_3
           );
}

inline const GLcolor decompressFromRGB16Bit(const GLcolorRGB16b &color)
{
    const GLfloat c_inv_63 = (1.0f / 63.0f);
    const GLfloat c_inv_31 = (1.0f / 31.0f);
    return GLcolor(
               float((color >> 11) & 0x001Fu) * c_inv_31,
               float((color >> 5) & 0x003Fu) * c_inv_63,
               float((color >> 0) & 0x001Fu) * c_inv_31
           );
}

} // namespace GL //

#endif // GLBITCOLOR_H //
