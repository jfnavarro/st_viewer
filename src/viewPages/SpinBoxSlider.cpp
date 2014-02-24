
#include "SpinBoxSlider.h"

#include <QHBoxLayout>
#include <QSpinBox>

#include "qxtspanslider.h"


SpinBoxSlider::SpinBoxSlider(QWidget *parent)
    : QWidget(parent)
{

  m_layout = new QHBoxLayout(this);
  m_left_spinbox = new QSpinBox(this);
  m_right_spinbox = new QSpinBox(this);
  m_spanslider = new QxtSpanSlider(this);
  m_spanslider->setMinimumWidth(200);
  m_spanslider->setOrientation(Qt::Horizontal);
  m_spanslider->setTickPosition(QSlider::TicksAbove);
  m_spanslider->setHandleMovementMode(QxtSpanSlider::NoOverlapping);

  setLowerValuePrivate(0);
  setUpperValuePrivate(100);


  m_layout->addWidget(m_left_spinbox);
  m_layout->addWidget(m_spanslider);
  m_layout->addWidget(m_right_spinbox);

  QObject::connect(m_spanslider, &QxtSpanSlider::lowerValueChanged, this, &SpinBoxSlider::setLowerValue); 
  QObject::connect(m_spanslider, &QxtSpanSlider::upperValueChanged, this, &SpinBoxSlider::setUpperValue);
  QObject::connect(m_right_spinbox, SIGNAL(valueChanged(int)), this, SLOT(setUpperValue(int)));   
  QObject::connect(m_left_spinbox, SIGNAL(valueChanged(int)), this, SLOT(setLowerValue(int)));   

  setLayout(m_layout);

  //    onInit();
}

void SpinBoxSlider::setToolTip(const QString &str) {
  m_spanslider->setToolTip(str);
}

void SpinBoxSlider::setMaximumValue(int value) {
  m_spanslider->setMaximum(value);
  m_left_spinbox->setMaximum(value);
  m_right_spinbox->setMaximum(value);
}

void SpinBoxSlider::setMinimumValue(int value) {
  m_spanslider->setMinimum(value);
  m_left_spinbox->setMinimum(value);
  m_right_spinbox->setMinimum(value);
}

void SpinBoxSlider::setLowerValuePrivate(int value) {
  m_lower_value = value;
  m_spanslider->setLowerPosition(value);
  m_spanslider->setLowerValue(value);
  m_left_spinbox->setValue(value);
  m_right_spinbox->setMinimum(value + 1);
  emit lowerValueChanged(value);
}

void SpinBoxSlider::setUpperValuePrivate(int value) {
  m_upper_value = value;
  m_spanslider->setUpperPosition(value);
  m_spanslider->setUpperValue(value);
  m_right_spinbox->setValue(value);
  m_left_spinbox->setMaximum(value - 1);
  emit upperValueChanged(value);
}

void SpinBoxSlider::setLowerValue(int value) {
  if (value != m_lower_value) {
    setLowerValuePrivate(value);
   }
}


void SpinBoxSlider::setUpperValue(int value) {
  if (value != m_upper_value) {
    setUpperValuePrivate(value);
   }
}


void SpinBoxSlider::setTickInterval(int stepLength) {
  m_spanslider->setTickInterval(stepLength);
  m_left_spinbox->setSingleStep(stepLength);
  m_right_spinbox->setSingleStep(stepLength);
}

SpinBoxSlider::~SpinBoxSlider()
{
}