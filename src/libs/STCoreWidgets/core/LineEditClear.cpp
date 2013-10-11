/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QDebug>
#include "utils/DebugHelper.h"

#include <QPaintEvent>
#include <QToolButton>
#include <QEvent>
#include "utils/DebugHelper.h"
#include "LineEditClearPrivate.h"

LineEditClearPrivate::LineEditClearPrivate(LineEditClear* pub)
    : LineEditButtonPrivate(pub), m_visible(false)
{
    
}

void LineEditClearPrivate::_q_textChanged(const QString& text)
{
    Q_Q(LineEditClear);
    if (text.isEmpty())
    {
        q->setButtonPosition(LineEditButton::None);
    }
    else
    {
        q->setButtonPosition(LineEditButton::RightInside);
    }
}
void LineEditClearPrivate::_q_clearRequested()
{
    
    Q_Q(LineEditClear);
    if (!q->isReadOnly())
    {
        q->clear();
    }
}
void LineEditClearPrivate::retranslateUi()
{
    Q_Q(LineEditClear);
    q->button()->setToolTip(q->tr("Clear"));
}

LineEditClear::LineEditClear(QWidget* parent)
    : LineEditButton(*new LineEditClearPrivate(this), parent)
{
    qDebug() << "um?!";
    Q_D(LineEditClear);
    qDebug() << d;
//     setButtonPosition(RightInside);
    setButtonPosition(LineEditButton::None);
    setIcon(QIcon(QStringLiteral(":/images/clear.png")));
    setAutoRaise(true);
    setButtonFocusPolicy(Qt::NoFocus);
    connect(this, SIGNAL(buttonClicked()), this, SLOT(_q_clearRequested()));
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(_q_textChanged(const QString&)));

//     Q_D(LineEditClear);
    d->retranslateUi();
}

void LineEditClear::paintEvent(QPaintEvent* event)
{
    Q_D(LineEditClear);
    // hack to make sure the button is disabled when the widget is made read only
    if (!d->m_visible && isReadOnly())
    {
        // widget became read only
        button()->setVisible(false);
        d->m_visible = true;
    } else if (d->m_visible && !isReadOnly())
    {
        // widget stopped being read only
        button()->setVisible(!text().isEmpty());
        d->m_visible = false;
    }
    LineEditButton::paintEvent(event);
}

void LineEditClear::setButtonVisible(bool visible)
{
    if (visible && isReadOnly())
    {
        return;
    }
    LineEditButton::setButtonVisible(visible);
}

void LineEditClear::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::EnabledChange)
    {
        setButtonVisible(isEnabled() && !text().isEmpty());
    }
    if (event->type() == QEvent::LanguageChange)
    {
        Q_D(LineEditClear);
        d->retranslateUi();
    }
    LineEditButton::changeEvent(event);
}

#include "moc_LineEditClear.cpp"
