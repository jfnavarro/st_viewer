/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QDebug>
#include "WidgetGL.h"

WidgetGL::WidgetGL(QWidget* parent, const QGLWidget* shareWidget, Qt::WindowFlags f)
    : QGLWidget(parent, shareWidget, f)
{

}

WidgetGL::WidgetGL(QGLContext* context, QWidget* parent,
                   const QGLWidget* shareWidget, Qt::WindowFlags f)
    : QGLWidget(context, parent, shareWidget, f)
{

}

WidgetGL::WidgetGL(const QGLFormat& format, QWidget* parent,
                   const QGLWidget* shareWidget, Qt::WindowFlags f)
    : QGLWidget(format, parent, shareWidget, f)
{

}

WidgetGL::~WidgetGL()
{

}

void WidgetGL::resizeGL(int width, int height)
{
    //devicePixelRatio() fix the problem with MAC retina
    qreal pixelRatio = devicePixelRatio();
    QGLWidget::resizeGL(width * pixelRatio, height * pixelRatio);
}
