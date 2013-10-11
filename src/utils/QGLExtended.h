/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef QGLEXTENDED_H
#define QGLEXTENDED_H

#include <qgl.h>

#include <QImage>

// functions in the QtExt namespace provide extensions to existing Qt
// functionality.
namespace QtExt
{
    // The QGLWidget::convertToGLFormat is broken as it throws an assertion
    // error when out of memory. Modified version that simply returns empty
    // image. Needed for Qt5.1.1 and before.
    const QImage convertToGLFormat(const QImage& img);

}

#endif // QGLEXTENDED_H //
