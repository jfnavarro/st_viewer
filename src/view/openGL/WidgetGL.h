/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef WIDGETGL_H
#define WIDGETGL_H

#include <QGLWidget>

// Extension of QGLWidget used to hook in on some of the standard opengl calls.
//NOTE Currently not used!
class WidgetGL : public QGLWidget
{
    Q_OBJECT
    
public:
    
    explicit WidgetGL(QWidget* parent=0,
                      const QGLWidget* shareWidget = 0, Qt::WindowFlags f=0);
    explicit WidgetGL(QGLContext *context, QWidget* parent=0,
                      const QGLWidget* shareWidget = 0, Qt::WindowFlags f=0);
    explicit WidgetGL(const QGLFormat& format, QWidget* parent=0,
                      const QGLWidget* shareWidget = 0, Qt::WindowFlags f=0);
    virtual ~WidgetGL();

    virtual void resizeGL(int width, int height);
};

#endif // WIDGETGL_H //
