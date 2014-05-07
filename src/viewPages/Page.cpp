/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QApplication>

#include "Page.h"

Page::Page(QWidget *parent) : QWidget(parent)
{
}

Page::~Page()
{
}

void Page::setWaiting(bool waiting)
{
    if (waiting) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    } else {
        QApplication::restoreOverrideCursor();
        QApplication::processEvents();
    }
}

void Page::increaseBar()
{
}

