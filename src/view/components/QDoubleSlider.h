/*
  Copyright (C) 2012  Spatial Transcriptomics AB, read LICENSE for licensing
  terms. Contact : Jose Fernandez Navarro
  <jose.fernandez.navarro@scilifelab.se>
  
*/

#ifndef QDOUBLESLIDER_H
#define QDOUBLESLIDER_H

#include <QSlider>
#include <QStyle>

class QStylePainter;
class QPoint;
class QWidget;
class QStyleOptionSlider;
class QMouseEvent;
class QPaintEvent;
class QRect;

class QDoubleSlider : public QSlider
{
    Q_OBJECT
    
    Q_PROPERTY(int lowerValue READ lowerValue WRITE setLowerValue)
    Q_PROPERTY(int upperValue READ upperValue WRITE setUpperValue)
    Q_PROPERTY(int lowerPosition READ lowerPosition WRITE setLowerPosition)
    Q_PROPERTY(int upperPosition READ upperPosition WRITE setUpperPosition)
    Q_PROPERTY(HandleMovementMode handleMovementMode READ handleMovementMode WRITE setHandleMovementMode)
    Q_ENUMS(HandleMovementMode)
    
public:
    
    explicit QDoubleSlider(QWidget* parent = 0);
    QDoubleSlider(Qt::Orientation orientation, QWidget* parent = 0);
    virtual ~QDoubleSlider();
    
    enum SpanHandle
    {
        NoHandle,
        LowerHandle,
        UpperHandle
    };
    
    enum HandleMovementMode
    {
        FreeMovement,
        NoCrossing,
        NoOverlapping
    };
    
    
    HandleMovementMode handleMovementMode() const;
    void setHandleMovementMode(HandleMovementMode mode);
    
    int lowerValue() const;
    int upperValue() const;
    
    int lowerPosition() const;
    int upperPosition() const;
    
    void initStyleOption(QStyleOptionSlider* option, SpanHandle handle = UpperHandle) const;
    
    int pick(const QPoint& pt) const;
    
    int pixelPosToRangeValue(int pos) const;
    
    void handleMousePress(const QPoint& pos, QStyle::SubControl& control, int value, SpanHandle handle);
    
    void drawHandle(QStylePainter* painter, SpanHandle handle) const;
    
    void drawSpan(QStylePainter* painter, const QRect& rect) const;
    
    int getLower() const;
    
    int getUpper() const;
    
    void setupPainter(QPainter* painter, Qt::Orientation orientation, qreal x1, qreal y1, qreal x2, qreal y2) const;

    void triggerAction(QAbstractSlider::SliderAction action, bool main);
    
    void swapControls();
    
public slots:
    
    void setLowerValue(int lower);
    void setUpperValue(int upper);
    void setSpan(int lower, int upper);
    void setLowerPosition(int lower);
    void setUpperPosition(int upper);
    
    void updateRange(int min, int max);
    void movePressedHandle();
    
signals:
    
    void spanChanged(int lower, int upper);
    void lowerValueChanged(int lower);
    void upperValueChanged(int upper);
    void lowerPositionChanged(int lower);
    void upperPositionChanged(int upper);

protected:
    
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void paintEvent(QPaintEvent* event);

    int lower;
    int upper;
    int lowerPos;
    int upperPos;
    int offset;
    int position;
    SpanHandle lastPressed;
    SpanHandle mainControl;
    QStyle::SubControl lowerPressed;
    QStyle::SubControl upperPressed;
    HandleMovementMode movement;
    bool firstMovement;
    bool blockTracking;
};


#endif	/* // QDOUBLESLIDER_H */
