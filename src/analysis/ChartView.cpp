#include "ChartView.h"

#include <QDebug>
#include <QMouseEvent>
#include <QFileDialog>
#include <QPdfWriter>
#include <QMessageBox>

static const QColor lasso_color = QColor(0,0,255,90);

ChartView::ChartView(QWidget *parent)
    : QChartView(parent)
    , m_panning(false)
    , m_lassoSelection(false)
{
    setChart(new QChart());
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

ChartView::~ChartView()
{

}

bool ChartView::viewportEvent(QEvent *event)
{
    return QChartView::viewportEvent(event);
}

void ChartView::mousePressEvent(QMouseEvent *event)
{
    const bool is_left = event->button() == Qt::LeftButton;
    const bool is_right = event->button() == Qt::RightButton;
    if (is_left) {
        m_panning = true;
        m_originPanning = event->pos();
        setCursor(Qt::ClosedHandCursor);
    } else if (is_right) {
        m_lassoSelection = true;
        m_lasso = QPainterPath();
        m_originLasso = event->pos();
        m_lasso.moveTo(m_originLasso);
    }
    QChartView::mousePressEvent(event);
}

void ChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_panning) {
        const QPoint point = event->pos();
        const QPointF pan_adjustment = QPointF(point - m_originPanning);
        chart()->scroll(-pan_adjustment.x(), pan_adjustment.y());
        m_originPanning = point;
    } else if (m_lassoSelection) {
        const QPoint new_point = event->pos();
        if ((new_point - m_originLasso).manhattanLength() > 5) {
            m_lasso.lineTo(new_point);
            m_originLasso = new_point;
            chart()->update(chart()->plotArea());
        }
    }
    QChartView::mouseMoveEvent(event);
}

void ChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_panning) {
        unsetCursor();
        m_panning = false;
    } else if (m_lassoSelection) {
        emit signalLassoSelection(m_lasso);
        m_lasso = QPainterPath();
        m_lassoSelection = false;
    }
    QChartView::mouseReleaseEvent(event);
}

void ChartView::wheelEvent(QWheelEvent *event)
{
    const int angle = event->angleDelta().y();
    const qreal factor = (angle > 0) ? 1.1 : 0.9;
    chart()->zoom(factor);
    QChartView::wheelEvent(event);
}

void ChartView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        chart()->zoomIn();
        break;
    case Qt::Key_Minus:
        chart()->zoomOut();
        break;
    case Qt::Key_Left:
        chart()->scroll(-10, 0);
        break;
    case Qt::Key_Right:
        chart()->scroll(10, 0);
        break;
    case Qt::Key_Up:
        chart()->scroll(0, 10);
        break;
    case Qt::Key_Down:
        chart()->scroll(0, -10);
        break;
    default:
        QChartView::keyPressEvent(event);
        break;
    }
}

void ChartView::drawForeground(QPainter *painter, const QRectF &rect)
{
    if (!m_lasso.isEmpty()) {
        painter->setBrush(lasso_color);
        painter->setPen(lasso_color);
        painter->drawPath(m_lasso.simplified());
    }
}

void ChartView::paintEvent(QPaintEvent *event)
{
    QChartView::paintEvent(event);
}

void ChartView::slotExportPlot(const QString &title)
{
    const QString filename = QFileDialog::getSaveFileName(this,
                                                          title,
                                                          QDir::homePath(),
                                                          QString("%1;;%2;;%3;;%4")
                                                          .arg(tr("JPEG Image Files (*.jpg *.jpeg)"))
                                                          .arg(tr("PNG Image Files (*.png)"))
                                                          .arg(tr("BMP Image Files (*.bmp)"))
                                                          .arg(tr("PDF Image Files (*.pdf)")));
    // early out
    if (filename.isEmpty()) {
        return;
    }

    const QFileInfo fileInfo(filename);
    const QFileInfo dirInfo(fileInfo.dir().canonicalPath());
    if (!fileInfo.exists() && !dirInfo.isWritable()) {
        QMessageBox::critical(this,
                              title,
                              tr("The file is not writable"));
        return;
    }

    const int quality = 100; // quality format (100 max, 0 min, -1 default)
    const QString format = fileInfo.suffix().toLower();
    QImage image = grab().toImage();
    if (format.toLower().contains("pdf")) {
        QPdfWriter writer(filename);
        const QPageSize size(image.size(), QPageSize::Unit::Millimeter, "custom");
        writer.setPageSize(size);
        writer.setResolution(25);
        writer.setPageMargins(QMarginsF(0,0,0,0));
        QPainter painter(&writer);
        painter.drawImage(0,0, image);
    } else if (!image.save(filename, format.toStdString().c_str(), quality)) {
        QMessageBox::critical(this,
                              title,
                              tr("The image could not be creted."));
    }
}
