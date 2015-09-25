/*
Copyright (C) 2012  Spatial Transcriptomics AB,
read LICENSE for licensing terms.
Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>
*/
#ifndef ST_ASSERTOPENGL_H
#define ST_ASSERTOPENGL_H

#include <QtGlobal>
#include <QString>

// Returns true if the OpenGL error code is GL_NO_ERROR, else false.
// Examines the OpenGL error code of the currently set OpenGL context. If the error code is not
// GL_NO_ERROR then a diagnostic message is sent via qDebug and the function returns false. If no
// context could be found then the function returns false. If the error code is GL_NO_ERROR then
// the function returns true with no messages.
// The OpenGL error code is cleared (reset to GL_NO_ERROR) by this call.
// See https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glGetError.xml
bool checkOpenGLNoError();

// Returns an explanation of the OpenGL error code. In many cases this may be very vague. The most
// frequent error is the GL_INVALID_ENUM. When an error occurs it is best to try to locate the
// OpenGL call that caused the context to move into an erroneous state, and consult the OpenGL
// standard. The no error code (GL_NO_ERROR) is accepted as a valid error code by this call. The
// returned explanation is in English and is based on the Khronos OpenGL standard text.
// See https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glGetError.xml
QString getOpenGLErrorCodeExplanation(const unsigned int errorCode);

// A macro which depends on the Q_ASSERT macro to be available and enabled. In the event of
// an non GL_NO_ERROR OpenGL error code being detected, a message describing the error code will be
// sent to the qDebug channel and then the Qt assert will be triggered. If the error code is
// GL_NO_ERROR then the assert will pass without any message being sent to qDebug.
// Use of this macro will cause the OpenGL error state to be cleared.
// The macro should be used liberally, ideally at the beginning and end of all functions which
// interact with the OpenGL context. Complex functions with many calls to OpenGL may want to
// use this macro on a line by line basis.
#define ASSERT_OPENGL_OK Q_ASSERT(checkOpenGLNoError())

#endif
