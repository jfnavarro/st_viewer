#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QChartView>
#include <QChart>
#include <QRubberBand>

QT_CHARTS_USE_NAMESPACE

// A simple wrapper around QChartView to allow zooming and mouse events
class ChartView : public QChartView
{
    Q_OBJECT

public:

    explicit ChartView(QWidget *parent = nullptr);
    virtual ~ChartView() override;

signals:

    void signalLassoSelection(QPainterPath);

public slots:

    void slotExportPlot(const QString &title);

protected:

    bool viewportEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;
    void paintEvent(QPaintEvent *event) override;

private:

    bool m_panning;
    bool m_lassoSelection;
    QPoint m_originPanning;
    QPoint m_originLasso;
    QPainterPath m_lasso;
};

#endif // CHARTVIEW_H
