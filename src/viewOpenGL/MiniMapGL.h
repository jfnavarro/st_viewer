/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef MINIMAPGL_H
#define MINIMAPGL_H

#include <QGLSceneNode>

class QGLPainter;
class QImage;
class QVector2DArray;
class QRectF;
class QColor;
class QEvent;

// MiniMap is an view port GUI item that visualizes the view ports current
// "image" in relation to the scene. Ie. it shows where in the scene the
// view port currently is.
class MiniMapGL : public QGLSceneNode
{
    Q_OBJECT

public:

    explicit MiniMapGL(QObject* parent = 0);
    virtual ~MiniMapGL();

    void setScene(const QRectF& scene);
    void setView(const QRectF& view);

    void setSceneColor(const QColor& sceneColor);
    const QColor& getSceneColor() const;
    void setViewColor(const QColor& viewColor);
    const QColor& getViewColor() const;

protected:

    void draw(QGLPainter *painter);
    void drawGeometry (QGLPainter * painter);

    bool event(QEvent *e);

signals:

    void pressed();
    void released();
    void clicked();
    void doubleClicked();

private:

    void updateTransform(const QRectF& scene);

    // mini versions
    QRectF m_scene;
    QRectF m_view;
    QColor m_sceneColor;
    QColor m_viewColor;
};

#endif // MINIMAPGL_H //
