/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLTYPETRAITS_H
#define GLTYPETRAITS_H

// std include
#include <float.h>

#include "GLCommon.h"

// GLTypeTraits contains multiple template and non-template structs that aim
// to provide opengl specific type info. A lot of the rendering functions
// in the opengl api requires type specific identifyers in the form of integer
// values (GLenum). The structs below provide a simple means eliminating the
// need to hardcode these value in the code.
//
// eg. the argument to the function below can be rewritten to hide the internal
// representation. Eliminating extensive refactoring if the representation is
// changed (GL_INT -> GL_FLOAT, GL_SHORT -> GL_UNSIGNED_SHORT, etc.).
//
//  GLoption *data = ...    GLoption *data = ...
//  func(data, GL_FLOAT) -> func(data, GLTypeTrait<GLoption>::type_enum)
namespace GL
{
// opengl type trait
// access to compile time type information give opengl type
template <typename T>
struct GLTypeTrait {
    static const GLenum type_enum;
    static const int type_size;
    static const T max_value;
    static const T min_value;
};

// opengl type enum trait
// access to compile time type information given opengl type enum value
template <int N>
struct GLTypeEnumTrait {
    static const int type_size;
};

// opengl type enum info
// access to run-time type information given opengl type enum value
struct GLTypeEnumInfo {
    static int type_size(const GLenum type);
    static inline bool valid_type(const GLenum type);
};

// opengl color enum traits
template <int N>
// access to compile time color information given opengl type enum value
struct GLColorEnumTrait {
    static const int color_count;
};

// opengl color enum info
// access to run-time color information given opengl type enum value
struct GLColorEnumInfo {
    static int color_count(const GLenum mode);
    static inline bool valid_color(const GLenum mode);
};

// opengl geometric type trait
// access to compile time geometric primitive information given opengl type enum value
template <int N>
struct GLGeomEnumTrait {
    static const int vertex_count;
};

// opengl geometric type info
// access to run-time geometric primitive information given opengl type enum value
struct GLGeomEnumInfo {
    static int vertex_count(const GLenum mode);
    static inline bool valid_geom(const GLenum mode);
};

} // namespace GL //

/****************************************** DEFINITION ******************************************/

namespace GL
{

template <typename T> const int GLTypeTrait<T>::type_size = sizeof(T);

inline bool GLTypeEnumInfo::valid_type(const GLenum type)
{
    return (GL_BYTE <= type && type <= GL_DOUBLE);
}

inline bool GLColorEnumInfo::valid_color(const GLenum mode)
{
    return (GL_COLOR_INDEX <= mode && mode <= GL_LUMINANCE_ALPHA);
}

inline bool GLGeomEnumInfo::valid_geom(const GLenum mode)
{
    return (GL_POINTS <= mode && mode <= GL_POLYGON);
}

} // namespace GL //

#endif // GLTYPETRAITS_H //
