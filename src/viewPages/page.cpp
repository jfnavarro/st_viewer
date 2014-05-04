/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QApplication>

#include "Page.h"

Page::Page(QWidget *parent) :
    QWidget(parent),
    m_progressDialog(nullptr),
    m_timer(nullptr),
    m_steps(0)
{
    m_progressDialog= new QProgressDialog("Downloading data.", "Cancel", 0, 100, this);
    m_progressDialog->setWindowModality(Qt::ApplicationModal);
    //TODO remove max/min buttons
    //m_progressDialog->setWindowFlags(Qt::CustomizeWindowHint |
    //                                 Qt::WindowTitleHint);
    m_progressDialog->setCancelButton(nullptr);
    //TODO implement abort logic
    //connect(m_progressDialog, SIGNAL(canceled()), this, SLOT(cancelProgressBar()));
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(increaseBar()));
}

void Page::setWaiting(bool waiting)
{
    if (waiting) {
        m_progressDialog->setValue(0);
        m_progressDialog->show();
        m_steps = 0;
        m_timer->start(100);
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    } else {
        m_timer->stop();
        m_progressDialog->hide();
        QApplication::restoreOverrideCursor();
    }
    QApplication::processEvents();
}

void Page::increaseBar()
{
    m_progressDialog->setValue(m_steps);
    if (++m_steps > m_progressDialog->maximum()) {
        m_timer->start(100);
        m_steps = 0;
    }
}
