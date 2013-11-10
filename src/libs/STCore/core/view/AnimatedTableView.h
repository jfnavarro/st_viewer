/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef ANIMATEDTABLEVIEW_H
#define ANIMATEDTABLEVIEW_H

#include <QTableView>

#include "WidgetBackgroundAnimation.h"

// Simple extension to the Qt table view which adds a background
// animation.
class AnimatedTableView : public QTableView
{
    Q_OBJECT

public:
    explicit AnimatedTableView(QWidget *parent = 0);
    virtual ~AnimatedTableView();

public slots:
    void setWaiting(const bool waiting);

protected:
    void paintEvent(QPaintEvent *event);

    WidgetBackgroundAnimation m_animation;
};

#endif // ANIMATEDTABLEVIEW_H //
