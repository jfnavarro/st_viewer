#ifndef SPINBOXSLIDER_H
#define SPINBOXSLIDER_H

#include <QWidget>
#include <QPointer>

class QSpinBox;
class QHBoxLayout;

// A simple widget with two spin boxes
// TODO add a double slider bar
class SpinBoxSlider : public QWidget
{
    Q_OBJECT
    Q_ENUMS(Controls)

public:
    enum Controls { onlySpinBoxes, onlySlider, sliderAndSpinBoxes };
    Q_DECLARE_FLAGS(ControlsFlags, Controls)

    explicit SpinBoxSlider(QWidget *parent = 0, ControlsFlags controlFlags = onlySpinBoxes);
    virtual ~SpinBoxSlider();

    // Some setters for the ticks
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
    QPointer<QSpinBox> m_left_spinbox;
    QPointer<QSpinBox> m_right_spinbox;
    QPointer<QHBoxLayout> m_layout;

    int m_upper_value;
    int m_lower_value;

    Q_DISABLE_COPY(SpinBoxSlider)
};
Q_DECLARE_OPERATORS_FOR_FLAGS(SpinBoxSlider::ControlsFlags)

#endif
