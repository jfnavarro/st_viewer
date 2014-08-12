/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef SETTIPS_H
#define SETTIPS_H

#include <qglobal.h>

QT_FORWARD_DECLARE_CLASS(QString)
QT_FORWARD_DECLARE_CLASS(QWidget)
QT_FORWARD_DECLARE_CLASS(QAction)

void setToolTipAndStatusTip(const QString &str, QWidget *widget);
void setToolTipAndStatusTip(const QString &str, QAction *action);

#endif // SETTIPS_H
