/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "LineEditButton.h"

#if !defined(ST_NO_LINEEDITBUTTON)

#include <QToolButton>
#include <QDebug>
#include <QActionEvent>
#include "LineEditButtonPrivate.h"

LineEditButtonPrivate::LineEditButtonPrivate(LineEditButton* pub)
    : ObjectPrivate(pub), m_button(0), m_position(LineEditButton::RightInside), m_visible(true)
{

}

void LineEditButtonPrivate::updateButtonPosition(int width)
{
    Q_Q(LineEditButton);
    QRect ncr = q->rect();

    switch (m_position)
    {
        case LineEditButton::None:
            break;
        case LineEditButton::RightInside:
            m_button->setGeometry(
                ncr.right() - width + 3, ncr.top() + 2,
                width - 4, ncr.height() - 4
            );
            break;
        case LineEditButton::RightOutside:
            m_button->setGeometry(
                ncr.right() - width + 1, ncr.top(),
                width, ncr.height()
            );
            break;
        case LineEditButton::LeftInside:
            m_button->setGeometry(
                ncr.left() + 2, ncr.top() + 2,
                width - 4, ncr.height() - 4
            );
            break;
        case LineEditButton::LeftOutside:
            m_button->setGeometry(
                ncr.left(), ncr.top(),
                width, ncr.height()
            );
            break;
        default:
            qDebug() << "[LineEditButton] Error: Unknown button position!";
    }

    // update visibility
    if (m_position != LineEditButton::None && m_visible)
    {
        m_button->show();
    }
    else
    {
        m_button->hide();
    }        
}

LineEditButton::LineEditButton(QWidget* parent)
    : QLineEdit(parent), Privatable(new LineEditButtonPrivate(this))
{
    Q_D(LineEditButton);
    d->m_button = new QToolButton(this);
    d->m_button->setCursor(Qt::ArrowCursor);
    connect(d->m_button, SIGNAL(clicked()), this, SIGNAL(buttonClicked()));
    setButtonPosition(RightOutside);
}

LineEditButton::LineEditButton(const QString& content, QWidget* parent)
    : QLineEdit(content, parent), Privatable(new LineEditButtonPrivate(this))
{
    Q_D(LineEditButton);
    d->m_button = new QToolButton(this);
    d->m_button->setCursor(Qt::ArrowCursor);
    connect(d->m_button, SIGNAL(clicked()), this, SIGNAL(buttonClicked()));
    setButtonPosition(RightOutside);
}

LineEditButton::LineEditButton(LineEditButtonPrivate& priv, QWidget* parent)
    : QLineEdit(parent), Privatable(&priv)
{
    Q_D(LineEditButton);
    d->m_button = new QToolButton(this);
    d->m_button->setCursor(Qt::ArrowCursor);
    connect(d->m_button, SIGNAL(clicked()), this, SIGNAL(buttonClicked()));
    setButtonPosition(RightOutside);
}

void LineEditButton::resizeEvent(QResizeEvent* event)
{
    Q_D(LineEditButton);
    int width = qMin(d->m_button->sizeHint().width(), height());
    d->updateButtonPosition(width);
    QLineEdit::resizeEvent(event);
}

QIcon LineEditButton::icon() const
{
    Q_D(const LineEditButton);
    return d->m_button->icon();
}

void LineEditButton::setIcon(const QIcon& icon)
{
    Q_D(LineEditButton);
    d->m_button->setIcon(icon);
}

void LineEditButton::setAutoRaise(bool autoRaise)
{
    Q_D(LineEditButton);
    d->m_button->setAutoRaise(autoRaise);
}

bool LineEditButton::autoRaise() const
{
    Q_D(const LineEditButton);
    return d->m_button->autoRaise();
}

LineEditButton::Position LineEditButton::buttonPosition() const
{
    Q_D(const LineEditButton);
    return d->m_position;
}

void LineEditButton::setButtonPosition(Position position)
{
    Q_D(LineEditButton);

    // early out
    if (d->m_position == position)
    {
        return;
    }

    d->m_position = position;
    int width = qMin(d->m_button->sizeHint().width(), testAttribute(Qt::WA_Resized) ? height() : sizeHint().height());

    switch (position)
    {
        case LineEditButton::None:
            setTextMargins(0,0,0,0);
            setContentsMargins(0,0,0,0);
            break;
        case LineEditButton::RightOutside:
            setTextMargins(0,0,0,0);
            setContentsMargins(0, 0, width+2, 0);
            break;
        case LineEditButton::RightInside:
            setContentsMargins(0,0,0,0);
            setTextMargins(0,0,width,0);
            break;
        case LineEditButton::LeftOutside:
            setTextMargins(0,0,0,0);
            setContentsMargins(width+2, 0, 0, 0);
            break;
        case LineEditButton::LeftInside:
            setContentsMargins(0,0,0,0);
            setTextMargins(width,0,0,0);
            break;
    }

    d->updateButtonPosition(width);
    update();
}

void LineEditButton::actionEvent(QActionEvent* event)
{
    Q_D(LineEditButton);

    switch (event->type())
    {
        case QEvent::ActionAdded:
            d->m_button->insertAction(event->before(), event->action());
            break;
        case QEvent::ActionRemoved:
            d->m_button->removeAction(event->action());
            break;
        case QEvent::ActionChanged:
        default:
            QLineEdit::actionEvent(event);
    }
}

void LineEditButton::setButtonVisible(bool visible)
{
    Q_D(LineEditButton);
    d->m_visible = visible;
    d->m_button->setVisible(visible && d->m_position != LineEditButton::None);
}
void LineEditButton::setButtonHidden(bool hidden)
{
    qDebug() << "[LineEditButton] setButtonHidden:" << hidden;
    setButtonVisible(!hidden);
}

bool LineEditButton::buttonIsVisible() const
{
    Q_D(const LineEditButton);
    return d->m_visible;
}

QToolButton* LineEditButton::button() const
{
   Q_D(const LineEditButton);
   return d->m_button;
}

Qt::FocusPolicy LineEditButton::buttonFocusPolicy() const
{
    return button()->focusPolicy();
}

void LineEditButton::setButtonFocusPolicy(Qt::FocusPolicy focusPolicy)
{
    button()->setFocusPolicy(focusPolicy);
}

#endif // NOT ST_NO_LINEEDITBUTTON //
