#include <QAbstractScrollArea>
#include <QTransform>

class CellGLView;

class ScrollArea : public QAbstractScrollArea
{
    Q_OBJECT

public:

    explicit ScrollArea(QWidget * parent=0 );
    virtual ~ScrollArea();
    CellGLView *cellGlView() const;
    virtual void setupViewport(QWidget *viewport) override;

public slots:

    void setCellGLViewScene(const QRectF scene);
    void setCellGLViewViewPort(const QRectF view);
    void setCellGLViewSceneTransformations(const QTransform transform);

protected:

    virtual void paintEvent( QPaintEvent * ) override;
    virtual void resizeEvent( QResizeEvent * ) override;

private slots:

  void someScrollChangedValue(int);

private:

    void adjustScrollBars();
    CellGLView *m_view;
    QWidget *m_container;
    QRectF m_cellglview_scene;
    QRectF m_cellglview_viewPort;
    QTransform    m_cellglview_sceneTransformations;
};
