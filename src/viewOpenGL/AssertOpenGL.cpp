/*
Copyright (C) 2015  Spatial Transcriptomics AB,
read LICENSE for licensing terms.
Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>
*/
#include <QDebug>
#include <QOpenGLFunctions>
#include "AssertOpenGL.h"

bool checkOpenGLError()
{
    QOpenGLFunctions *openglFuncs = QOpenGLContext::currentContext()->functions();

    if (nullptr != openglFuncs) {
        const auto errorCode = openglFuncs->glGetError();

        if (GL_NO_ERROR == errorCode) {
            return true;
        } else {
            qDebug() << "OpenGL error: " << getOpenGLErrorCodeExplanation(errorCode) << "\n";
            return false;
        }
    }

    qDebug() << "Error: No OpenGL context detected.\n";
    return false;
}

QString getOpenGLErrorCodeExplanation(const unsigned int errorCode)
{
    QString explanation;

    switch (errorCode) {
    case GL_NO_ERROR:
        explanation = "No error has been recorded.";
        break;

    case GL_INVALID_ENUM:
        explanation = "An unacceptable value is specified for an enumerated argument.";
        break;

    case GL_INVALID_VALUE:
        explanation = "A numeric argument is out of range.";
        break;

    case GL_INVALID_OPERATION:
        explanation = "The specified operation is not allowed in the current state.";
        break;

    case GL_INVALID_FRAMEBUFFER_OPERATION:
        explanation = "The framebuffer object is not complete.";
        break;

    case GL_OUT_OF_MEMORY:
        explanation = "There is not enough memory left to execute the command.";
        break;

    case GL_STACK_UNDERFLOW:
        explanation = "An attempt has been made to perform an operation that "
                      "would cause an internal stack to underflow.";
        break;

    case GL_STACK_OVERFLOW:
        explanation = "An attempt has been made to perform an operation that "
                      "would cause an internal stack to overflow.";
        break;

    default:
        explanation = "An unknown error code has been returned from 'glGetError'";
        break;
    }

    return explanation;
}
