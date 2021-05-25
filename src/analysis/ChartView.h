#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QChartView>
#include <QChart>
#include <QRubberBand>

//QT_CHARTS_USE_NAMESPACE

// A simple wrapper around QChartView to allow zooming, panning and selections
class ChartView : public QChartView
{
    Q_OBJECT

public:

    explicit ChartView(QWidget *parent = nullptr);
    virtual ~ChartView() override;

signals:

    // when the user has made a lasso selection
    void signalLassoSelection(QPainterPath);

public slots:

    // when the user wants to export the plot to a file
    void slotExportPlot(const QString &title);

protected:

    // overloaded methods to allow interaction with the view
    bool viewportEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;
    void paintEvent(QPaintEvent *event) override;

private:

    // variables used for the selection and panning
    bool m_panning;
    bool m_lassoSelection;
    QPoint m_originPanning;
    QPoint m_originLasso;
    QPainterPath m_lasso;
};

#endif // CHARTVIEW_H
