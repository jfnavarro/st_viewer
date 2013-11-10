/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLIMAGEWRITER_H
#define GLIMAGEWRITER_H

#include <GLCommon.h>
#include <GLColor.h>
#include <math/GLMath.h>
#include <image/GLImage.h>

namespace GL
{

// Simple writing class providing functionality for writing procedurally
// (or manually for that matter) generated images. Data is written in a
// linear fashion on a per pixel or per line approach.
//TODO imagewriter assumes GL_RGBA mode, should be made mode independent.
class GLimagewriter
{
public:
    enum GradientAngle
    {
        HorizontalGradient,
        VerticalGradient
    };

    inline explicit GLimagewriter(GLimage &image);
    inline ~GLimagewriter();

    const bool writePixel(const GLcolor &color);
    const bool writeLine(const GLcolor &color);

    // fills remaining pixel with specified color
    const bool writeFillColor(const GLcolor &color);
    // fills remaining pixels with gradient of specified two colors
    const bool writeFillGradient(const GLcolor &color0, const GLcolor &color1,
                                 const GradientAngle gradient = GLimagewriter::VerticalGradient);

    // convenience functions for generating prefilled images
    static inline const bool imageFillColor(GLimage &image, const GLcolor &color);
    static inline const bool imageFillGradient(GLimage &image, const GLcolor &color0, const GLcolor &color1,
                                               const GradientAngle gradient = GLimagewriter::VerticalGradient);

private:
    GLimage &m_image;
    GLsizei m_index;
    GLsizei m_indexEnd;
};

} // namespace //

/****************************************** DEFINITION ******************************************/

namespace GL
{

inline GLimagewriter::GLimagewriter(GLimage &image)
    : m_image(image), m_index(0), m_indexEnd(0)
{
    // assume specific mode and type
    //TODO implement mode and type independet image writing
    Q_ASSERT_X(image.mode() == GL_RGBA && image.type() == GL_FLOAT, "GLimagewriter", "Unsupported image mode or type!");

    // cache buffer size
    m_indexEnd = m_image.size();
}
inline GLimagewriter::~GLimagewriter()
{

}

inline const bool GLimagewriter::imageFillColor(GLimage &image, const GLcolor &color)
{
    GLimagewriter writer(image);
    return writer.writeFillColor(color);
}
inline const bool GLimagewriter::imageFillGradient(GLimage &image, const GLcolor &color0, 
                                                   const GLcolor &color1, const GradientAngle gradient)
{
    GLimagewriter writer(image);
    return writer.writeFillGradient(color0, color1, gradient);
}

} // namespace GL //

#endif // GLIMAGEWRITER_H //
