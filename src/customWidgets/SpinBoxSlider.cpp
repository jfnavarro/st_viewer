#include "SpinBoxSlider.h"

#include <QHBoxLayout>
#include <QSpinBox>
#include <QHBoxLayout>

SpinBoxSlider::SpinBoxSlider(QWidget *parent, ControlsFlags controlFlags)
    : QWidget(parent)
    , m_left_spinbox(nullptr)
    , m_right_spinbox(nullptr)
    , m_layout(nullptr)
    , m_upper_value(0)
    , m_lower_value(0)
{
    Q_UNUSED(controlFlags);

    m_layout = new QHBoxLayout(this);
    m_left_spinbox = new QSpinBox(this);
    m_right_spinbox = new QSpinBox(this);

    m_layout->addWidget(m_left_spinbox);
    m_layout->addWidget(m_right_spinbox);

    connect(m_right_spinbox, SIGNAL(valueChanged(int)), this, SLOT(slotSetUpperValue(int)));
    connect(m_left_spinbox, SIGNAL(valueChanged(int)), this, SLOT(slotSetLowerValue(int)));

    m_left_spinbox->setVisible(true);
    m_right_spinbox->setVisible(true);

    setLayout(m_layout);
}

SpinBoxSlider::~SpinBoxSlider()
{
}

void SpinBoxSlider::setMaximumValue(const unsigned value)
{
    // We block signals here as we only want
    // the signals to be emitted when the user
    // interacts with the component
    blockSignals(true);
    m_upper_value = value;
    m_left_spinbox->setMaximum(value);
    m_right_spinbox->setMaximum(value);
    m_right_spinbox->setValue(value);
    blockSignals(false);
}

void SpinBoxSlider::setMinimumValue(const unsigned value)
{
    // We block signals here as we only want
    // the signals to be emitted when the user
    // interacts with the component
    blockSignals(true);
    m_lower_value = value;
    m_left_spinbox->setMinimum(value);
    m_right_spinbox->setMinimum(value);
    m_left_spinbox->setValue(value);
    blockSignals(false);
}

void SpinBoxSlider::slotSetLowerValue(const int value)
{
    const unsigned tmp_value = static_cast<unsigned>(value);
    if (tmp_value != m_lower_value) {
        m_lower_value = tmp_value;
        m_left_spinbox->setValue(m_lower_value);
        emit signalLowerValueChanged(m_lower_value);
    }
}

void SpinBoxSlider::slotSetUpperValue(const int value)
{
    const unsigned tmp_value = static_cast<unsigned>(value);
    if (tmp_value != m_upper_value) {
        m_upper_value = tmp_value;
        m_right_spinbox->setValue(m_upper_value);
        emit signalUpperValueChanged(m_upper_value);
    }
}

void SpinBoxSlider::setTickInterval(const unsigned stepLength)
{
    m_left_spinbox->setSingleStep(stepLength);
    m_right_spinbox->setSingleStep(stepLength);
}
