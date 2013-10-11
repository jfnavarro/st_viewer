/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#if defined(QT_NO_TOOLBUTTON) || defined(QT_NO_LINEEDIT)
#define ST_NO_LINEEDITBUTTON
#endif

#ifndef LINEEDITBUTTON_H
#define LINEEDITBUTTON_H

#ifndef ST_NO_LINEEDITBUTTON

#include <QLineEdit>
#include <QIcon>

#include "Core.h"

class QResizeEvent;
class QActionEvent;
class QToolButton;

//NOTE Core provides an adaptation of the widgets as designed by wwWidgets:
// http://www.wysota.eu.org/wwwidgets/doc/html/index.html

// LineEditBotton extends QLineEdit to provide a line edit widget with an
// associated button. Convenient functionality is provided to position the
// button in relation to the line edit field.
class LineEditButtonPrivate;
class LineEditButton : public QLineEdit, public Privatable
{
    Q_OBJECT
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
    Q_PROPERTY(bool autoRaise READ autoRaise WRITE setAutoRaise)
    Q_ENUMS(Position)
    Q_PROPERTY(Position buttonPosition READ buttonPosition WRITE setButtonPosition)
    Q_PROPERTY(bool buttonVisible READ buttonIsVisible WRITE setButtonVisible)
    
public:
    
    enum Position { None = 0, RightOutside = 1, RightInside = 2, LeftOutside = 3, LeftInside = 4 };
    
    explicit LineEditButton(QWidget* parent = 0);
    
    LineEditButton(const QString& content, QWidget* parent = 0);

    QIcon icon() const;
    bool autoRaise() const;
    Position buttonPosition() const;
    void setButtonPosition(Position pos);
    bool buttonIsVisible() const;

    Qt::FocusPolicy buttonFocusPolicy() const;
    void setButtonFocusPolicy(Qt::FocusPolicy);

public slots:
    
    void setIcon(const QIcon& icon);
    void setAutoRaise(bool autoRaise);
    void setButtonVisible(bool visible);
    void setButtonHidden(bool hidden);

signals:
    
    void buttonClicked();

protected:
    
    LineEditButton(LineEditButtonPrivate& priv, QWidget* parent);

    void resizeEvent(QResizeEvent *ev);
    void actionEvent(QActionEvent *ev);
    QToolButton* button() const;

private:

    Q_DISABLE_COPY(LineEditButton);
    ST_DECLARE_PRIVATE(LineEditButton);
};

#endif // not ST_NO_LINEEDITBUTTON //
#endif // LINEEDITBUTTON_H //
