/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef SPINBOXSLIDER_H
#define SPINBOXSLIDER_H

#include <QWidget>
#include <QPointer>

class QSpinBox;
class QxtSpanSlider;
class QHBoxLayout;

// Wrapper around QxtSpanSlider to add two spin boxes one on each side
// We can control whether to show only the slider, the spin boxes or both
class SpinBoxSlider : public QWidget
{
    Q_OBJECT
    Q_ENUMS(Controls)

public:
    enum Controls { onlySpinBoxes, onlySlider, sliderAndSpinBoxes };
    Q_DECLARE_FLAGS(ControlsFlags, Controls)

    explicit SpinBoxSlider(QWidget* parent = 0, ControlsFlags controlFlags = onlySpinBoxes);
    virtual ~SpinBoxSlider();

    // Some setters for the ticks
    void setToolTip(const QString& str);
    void setMaximumValue(const int max);
    void setMinimumValue(const int min);
    void setTickInterval(const int interval);

signals:

    void signalLowerValueChanged(int);
    void signalUpperValueChanged(int);

public slots:

    void slotSetLowerValue(const int min);
    void slotSetUpperValue(const int max);

private:
    QPointer<QxtSpanSlider> m_spanslider;
    QPointer<QSpinBox> m_left_spinbox;
    QPointer<QSpinBox> m_right_spinbox;
    QPointer<QHBoxLayout> m_layout;

    int m_upper_value;
    int m_lower_value;

    Q_DISABLE_COPY(SpinBoxSlider)
};
Q_DECLARE_OPERATORS_FOR_FLAGS(SpinBoxSlider::ControlsFlags)

#endif
