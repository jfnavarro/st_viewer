#ifndef SCROLLAREA_H
#define SCROLLAREA_H

#include <QAbstractScrollArea>
#include <QTransform>

class CellGLView;

class ScrollArea : public QAbstractScrollArea
{
    Q_OBJECT

public:

    explicit ScrollArea(QWidget * parent = 0 );
    virtual ~ScrollArea();

    CellGLView *cellGlView() const;
    virtual void setupViewport(QWidget *viewport) override;

public slots:

    void setCellGLViewScene(const QRectF scene);
    void setCellGLViewViewPort(const QRectF view);
    void setCellGLViewSceneTransformations(const QTransform transform);

protected:

    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

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
    const int m_scrollBarSteps = 100000;

    CellGLView* m_view = nullptr;
    QWidget* m_container = nullptr;
    QRectF m_cellglview_scene;
    QRectF m_cellglview_viewPort;
    QTransform  m_cellglview_sceneTransformations;

    Q_DISABLE_COPY(ScrollArea)
};
#endif //SCROLLAREA_H
