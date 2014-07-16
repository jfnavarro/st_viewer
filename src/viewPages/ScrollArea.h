/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef SCROLLAREA_H
#define SCROLLAREA_H

#include <QAbstractScrollArea>
#include <QTransform>
#include <QPointer>

class CellGLView;

//Scroll Area is a wrapper around CellGLView
//as part of the CellView, ScrollArea adds
//horizonal and vertical scroll bars
class ScrollArea : public QAbstractScrollArea
{
    Q_OBJECT

public:

    explicit ScrollArea(QWidget *parent = 0);
    virtual ~ScrollArea();

    void initializeView(QPointer<CellGLView> view);

    void setupViewport(QWidget *viewport) override;

public slots:

    void setCellGLViewScene(const QRectF scene);
    void setCellGLViewViewPort(const QRectF view);
    void setCellGLViewSceneTransformations(const QTransform transform);

protected:

    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:

  void someScrollBarChangedValue(int);

private:

    static void adjustScrollBar(const int scrollBarSteps,
                                const qreal value,
                                const qreal value_minimum,
                                const qreal value_range,
                                const qreal viewPortInSceneCoordinatesRange,
                                QScrollBar *scrollBar);

    void adjustScrollBars();

    //TODO magic number?
    static const int m_scrollBarSteps = 100000;

    QPointer<CellGLView> m_view;
    QPointer<QWidget> m_container;
    QRectF m_cellglview_scene;
    QRectF m_cellglview_viewPort;
    QTransform  m_cellglview_sceneTransformations;

    Q_DISABLE_COPY(ScrollArea)
};
#endif //SCROLLAREA_H
