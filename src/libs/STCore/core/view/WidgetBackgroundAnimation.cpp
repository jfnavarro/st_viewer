/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QWidget>
#include <QMovie>

#include <QPaintEvent>
#include <QPixmap>
#include <QRect>
#include <QPainter>

#include <QAbstractScrollArea>

#include "WidgetBackgroundAnimation.h"

WidgetBackgroundAnimation::WidgetBackgroundAnimation(QWidget *displayWidget):
    m_displayWidget(displayWidget), m_movie(0), m_visible(false), m_enable(false)
{
    //NOTE widgets based on QAbstractScrollArea rely on a viewport for rendering
    QAbstractScrollArea *scrollArea = dynamic_cast<QAbstractScrollArea *>(m_displayWidget);
    if (scrollArea)
    {
        m_displayWidget = scrollArea->viewport();
    }
}
WidgetBackgroundAnimation::~WidgetBackgroundAnimation()
{
    // cleanup
    if (m_movie)
    {
        delete m_movie;
    }
}

void WidgetBackgroundAnimation::setAnimation(const QString &fileName)
{
    // cleanup
    if (m_movie)
    {
        delete m_movie;
    }
    m_movie = new QMovie(fileName);
    connect(m_movie, SIGNAL(frameChanged(int)), m_displayWidget, SLOT(repaint()));
}

void WidgetBackgroundAnimation::setVisible(const bool visible)
{
    m_visible = visible;
    m_displayWidget->repaint();
}

void WidgetBackgroundAnimation::setEnabled(const bool enabled)
{
    m_enable = enabled;
    
    // early out
    if (!m_movie)
    {
        return;
    }

    if (enabled)
    {
        m_movie->start();
    }
    else
    {
        m_movie->stop();
    }
}

void WidgetBackgroundAnimation::paintAnimation(QPaintEvent *event)
{
    // early out
    if (!m_movie)
    {
        return;
    }

    if (m_visible && m_displayWidget->isVisible())
    {
        const QPixmap movieFrame = m_movie->currentPixmap();

        QRect movieRect = movieFrame.rect();
        movieRect.moveCenter(m_displayWidget->rect().center());

        const QRect eventRect = event->rect();
        if (movieRect.intersects(eventRect))
        {
            QPainter painter(m_displayWidget);
            painter.drawPixmap(movieRect.left(), movieRect.top(), movieFrame);
        }
    }
}

const bool WidgetBackgroundAnimation::enabled() const
{
    return m_enable;
}

const bool WidgetBackgroundAnimation::visible() const
{
    return m_visible;
}

