/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLFLOAT_H
#define GLFLOAT_H

#include "GLCommon.h"

namespace GL
{
// the following namespace functions implement bit manipulation
// functionality for GLfloat variables that emulates that of an
// integer.
// NOTE this does not manipulate the actual bits of the
// float!
// Due to precision loss only 2^23 bits are actually available.

inline const GLfloat bitSet(const GLfloat bitFloat, const GLuint bit);
inline const GLfloat bitClear(const GLfloat bitFloat, const GLuint bit);
inline const bool bitTest(const GLfloat bitFloat, const GLuint bit);

} // namespace GL //

/****************************************** DEFINITION ******************************************/

namespace GL
{
// implement using float -> uint -> float conversion

inline const GLfloat bitSet(const GLfloat bitFloat, const GLuint bit)
{
    return GLfloat( GLuint(bitFloat) | (1u << bit) );
}
inline const GLfloat bitClear(const GLfloat bitFloat, const GLuint bit)
{
    return GLfloat( GLuint(bitFloat) & ~(1u << bit) );
}
inline const bool bitTest(const GLfloat bitFloat, const GLuint bit)
{
    return (GLuint(bitFloat) & (1u << bit)) != 0u;
}

} // namespace GL //

#endif // GLFLOAT_H //
