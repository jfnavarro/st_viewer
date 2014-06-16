#ifndef SCROLLAREA_H
#define SCROLLAREA_H

#include <QAbstractScrollArea>
#include <QTransform>

class CellGLView;

//Scroll Area is a wrapper around CellGLView
//as part of the CellView, ScrollArea adds
//horizonal and vertical scroll bars
class ScrollArea : public QAbstractScrollArea
{
    Q_OBJECT

public:

    explicit ScrollArea(QWidget *parent = 0 );
    virtual ~ScrollArea();

    void initializeView(QSharedPointer<CellGLView> view);

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

    //TOFIX magic number?
    static const int m_scrollBarSteps = 100000;

    //TODO this causes seg faults in the destructor
    QSharedPointer<CellGLView> m_view;
    QScopedPointer<QWidget> m_container;
    QRectF m_cellglview_scene;
    QRectF m_cellglview_viewPort;
    QTransform  m_cellglview_sceneTransformations;

    Q_DISABLE_COPY(ScrollArea)
};
#endif //SCROLLAREA_H
