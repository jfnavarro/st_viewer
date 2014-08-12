/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "utils/setTips.h"

#include <QString>
#include <QWidget>
#include <QAction>

void setToolTipAndStatusTip(const QString &str, QWidget *widget) {
  Q_ASSERT(widget);
  widget->setToolTip(str);
  widget->setStatusTip(str);
}

void setToolTipAndStatusTip(const QString &str, QAction *action) {
  Q_ASSERT(action);
  action->setToolTip(str);
  action->setStatusTip(str);
}
