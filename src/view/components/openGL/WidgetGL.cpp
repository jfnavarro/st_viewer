/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "WidgetGL.h"

WidgetGL::WidgetGL(QWidget* parent,
                   const QGLWidget* shareWidget, Qt::WindowFlags f)
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
    //TODO devicePixelRatio() will be directly exposed to QGLWidget in next QT release
//     qreal pixelRatio = devicePixelRatio();
//     qreal pixelRatio = windowHandle()->devicePixelRatio();
//     qDebug() << QString("[WidgetGL] devicePixelRatio: %1").arg(pixelRatio);
//     QGLWidget::resizeGL(width*pixelRatio, height*pixelRatio);
    QGLWidget::resizeGL(width, height);
}
