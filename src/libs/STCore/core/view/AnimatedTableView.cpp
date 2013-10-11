/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "AnimatedTableView.h"

AnimatedTableView::AnimatedTableView(QWidget *parent)
    : QTableView(parent), m_animation(this)
{
    m_animation.setAnimation(":images/loader.gif");
}
AnimatedTableView::~AnimatedTableView()
{

}

void AnimatedTableView::setWaiting(const bool waiting)
{
    m_animation.setEnabled(waiting);
    m_animation.setVisible(waiting);
}

void AnimatedTableView::paintEvent(QPaintEvent *event)
{
    
    QTableView::paintEvent(event);

    m_animation.paintAnimation(event);
}
