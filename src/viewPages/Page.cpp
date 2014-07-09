/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QApplication>
#include <QMessageBox>

#include "Page.h"

Page::Page(QWidget *parent) :
    QWidget(parent),
    m_progressDialog(nullptr),
    m_timer(nullptr),
    m_steps(0)
{
    m_progressDialog= new QProgressDialog("Downloading data...", "Cancel", 0, 100, this);
    m_progressDialog->setWindowModality(Qt::ApplicationModal);
    //TODO remove max/min buttons
    //m_progressDialog->setWindowFlags(Qt::CustomizeWindowHint |
    //                                 Qt::WindowTitleHint);
    m_progressDialog->setCancelButton(nullptr);
    m_progressDialog->setAutoClose(false);
    //TODO implement abort logic
    //connect(m_progressDialog, SIGNAL(canceled()), this, SLOT(cancelProgressBar()));
    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()), this, SLOT(increaseBar()));
}

Page::~Page()
{
   m_progressDialog->deleteLater();
   m_progressDialog = nullptr;

   m_timer->deleteLater();
   m_timer = nullptr;
}

void Page::setWaiting(bool waiting)
{
    //TODO check consistence of this
    if (waiting) {
        m_progressDialog->setValue(0);
        m_progressDialog->show();
        m_progressDialog->raise();
        m_progressDialog->activateWindow();
        m_steps = 0;
        m_timer->start(100);
    } else {
        m_timer->stop();
        m_progressDialog->cancel();
        m_progressDialog->hide();
    }
}

void Page::increaseBar()
{
    m_progressDialog->setValue(m_steps);
    if (++m_steps > m_progressDialog->maximum()) {
        m_timer->start(100);
        m_steps = 0;
        m_progressDialog->setValue(0);
    }
}

void Page::showWarning(const QString &header, const QString &body)
{
    QMessageBox::warning(this, header, body);
}

void Page::showError(const QString &header, const QString &body)
{
    QMessageBox::critical(this, header, body);
}
