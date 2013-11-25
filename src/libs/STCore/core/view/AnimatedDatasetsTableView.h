/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef ANIMATEDDATASETSTABLEVIEW_H
#define ANIMATEDDATASETSTABLEVIEW_H

#include <QTableView>
#include "WidgetBackgroundAnimation.h"

class DatasetItemModel;

// Simple extension to the Qt table view which adds a background
// animation.
class AnimatedDatasetsTableView : public QTableView
{
    Q_OBJECT

public:
    explicit AnimatedDatasetsTableView(QWidget *parent = 0);
    virtual ~AnimatedDatasetsTableView();

public slots:
    void setWaiting(const bool waiting);

protected:
    void paintEvent(QPaintEvent *event);

    DatasetItemModel *m_datasetModel;
    WidgetBackgroundAnimation m_animation;
};

#endif // ANIMATEDDATASETSTABLEVIEW_H //
