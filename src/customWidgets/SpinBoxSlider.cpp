#include "SpinBoxSlider.h"

#include <QHBoxLayout>
#include <QSpinBox>
#include <QHBoxLayout>

#include "qxtspanslider.h"

SpinBoxSlider::SpinBoxSlider(QWidget* parent, ControlsFlags controlFlags)
    : QWidget(parent)
    , m_spanslider(nullptr)
    , m_left_spinbox(nullptr)
    , m_right_spinbox(nullptr)
    , m_layout(nullptr)
    , m_upper_value(0)
    , m_lower_value(0)
{
    m_layout = new QHBoxLayout(this);
    m_left_spinbox = new QSpinBox(this);
    m_right_spinbox = new QSpinBox(this);
    m_spanslider = new QxtSpanSlider(this);

    m_spanslider->setMinimumWidth(200);
    m_spanslider->setOrientation(Qt::Horizontal);
    m_spanslider->setTickPosition(QSlider::TicksAbove);
    m_spanslider->setHandleMovementMode(QxtSpanSlider::NoOverlapping);

    m_layout->addWidget(m_left_spinbox);
    m_layout->addWidget(m_spanslider);
    m_layout->addWidget(m_right_spinbox);

    // restrict interface
    m_left_spinbox->setVisible(false);
    m_spanslider->setVisible(false);
    m_right_spinbox->setVisible(false);

    if (controlFlags.testFlag(Controls::onlySlider)
        || controlFlags.testFlag(Controls::sliderAndSpinBoxes)) {
        connect(m_spanslider.data(),
                &QxtSpanSlider::lowerValueChanged,
                this,
                &SpinBoxSlider::slotSetLowerValue);
        connect(m_spanslider.data(),
                &QxtSpanSlider::upperValueChanged,
                this,
                &SpinBoxSlider::slotSetUpperValue);
        m_spanslider->setVisible(true);
    }

    if (controlFlags.testFlag(Controls::onlySpinBoxes)
        || controlFlags.testFlag(Controls::sliderAndSpinBoxes)) {
        connect(m_right_spinbox, SIGNAL(valueChanged(int)), this, SLOT(slotSetUpperValue(int)));
        connect(m_left_spinbox, SIGNAL(valueChanged(int)), this, SLOT(slotSetLowerValue(int)));
        m_left_spinbox->setVisible(true);
        m_right_spinbox->setVisible(true);
    }

    setLayout(m_layout);
}

SpinBoxSlider::~SpinBoxSlider()
{
}

void SpinBoxSlider::setToolTip(const QString& str)
{
    m_spanslider->setToolTip(str);
}

void SpinBoxSlider::setMaximumValue(const int value)
{
    // We block signals here as we only want
    // the signals to be emitted when the user
    // interacts with the component
    blockSignals(true);
    m_upper_value = value;

    m_spanslider->setMaximum(value);
    m_spanslider->setUpperPosition(value);
    m_spanslider->setUpperValue(value);

    m_left_spinbox->setMaximum(value);
    m_right_spinbox->setMaximum(value);
    m_right_spinbox->setValue(value);
    blockSignals(false);
}

void SpinBoxSlider::setMinimumValue(const int value)
{
    // We block signals here as we only want
    // the signals to be emitted when the user
    // interacts with the component
    blockSignals(true);
    m_lower_value = value;

    m_spanslider->setMinimum(value);
    m_spanslider->setLowerPosition(value);
    m_spanslider->setLowerValue(value);

    m_left_spinbox->setMinimum(value);
    m_right_spinbox->setMinimum(value);
    m_left_spinbox->setValue(value);
    blockSignals(false);
}

void SpinBoxSlider::slotSetLowerValue(const int value)
{
    if (value != m_lower_value) {
        m_lower_value = value;
        m_spanslider->setLowerPosition(value);
        m_spanslider->setLowerValue(value);
        m_left_spinbox->setValue(value);
        emit signalLowerValueChanged(value);
    }
}

void SpinBoxSlider::slotSetUpperValue(const int value)
{
    if (value != m_upper_value) {
        m_upper_value = value;
        m_spanslider->setUpperPosition(value);
        m_spanslider->setUpperValue(value);
        m_right_spinbox->setValue(value);
        emit signalUpperValueChanged(value);
    }
}

void SpinBoxSlider::setTickInterval(const int stepLength)
{
    m_spanslider->setTickInterval(stepLength);
    m_left_spinbox->setSingleStep(stepLength);
    m_right_spinbox->setSingleStep(stepLength);
}
