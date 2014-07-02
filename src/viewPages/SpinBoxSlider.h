/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef SPINBOXSLIDER_H
#define SPINBOXSLIDER_H

#include <QWidget>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QPointer>

#include "qxtspanslider.h"

//TODO move to folder Widgets(create if it does not exist)

//Wrapper around QxtSpanSlider
//to add two spin boxes one on each side
class SpinBoxSlider : public QWidget
{
    Q_OBJECT

public:
    
    explicit SpinBoxSlider(QWidget *parent = 0);
    virtual ~SpinBoxSlider();

    void setToolTip(const QString &str);
    void setMaximumValue(const int max);
    void setMinimumValue(const int min);
    void setTickInterval(const int interval);

signals:

    void lowerValueChanged(int);
    void upperValueChanged(int);

public slots:
    
    void setLowerValue(const int min);
    void setUpperValue(const int max);

private:

    void setLowerValuePrivate(const int min);
    void setUpperValuePrivate(const int max);

    QPointer<QxtSpanSlider> m_spanslider;
    QPointer<QSpinBox> m_left_spinbox;
    QPointer<QSpinBox> m_right_spinbox;
    QPointer<QHBoxLayout> m_layout;

    int m_upper_value;
    int m_lower_value;

    Q_DISABLE_COPY(SpinBoxSlider)
};

#endif


