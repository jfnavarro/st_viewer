/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QApplication>
#include <QMessageBox>
#include <QStyle>
#include <QDesktopWidget>

#include "Page.h"

Page::Page(QWidget *parent) :
    QWidget(parent),
    m_progressDialog(nullptr),
    m_timer(nullptr)
{
    m_progressDialog = new QProgressDialog(this);
    m_progressDialog->setWindowModality(Qt::WindowModal);
    m_progressDialog->setModal(true);
    m_progressDialog->setWindowFlags(m_progressDialog->windowFlags()
                                     & ~Qt::WindowContextHelpButtonHint
                                     & ~Qt::WindowCancelButtonHint
                                     & ~Qt::WindowCloseButtonHint
                                     & ~Qt::WindowMinMaxButtonsHint);
    m_progressDialog->setCancelButton(nullptr);
    m_progressDialog->setAutoClose(false);
    m_progressDialog->setRange(0,0);

    //TODO implement abort logic
    //connect(m_progressDialog, SIGNAL(canceled()), this, SLOT(cancelProgressBar()));
    m_timer = new QTimer(this);
    m_timer->setInterval(500);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(increaseBar()));
}

Page::~Page()
{
    m_progressDialog->deleteLater();
    m_progressDialog = nullptr;

    m_timer->deleteLater();
    m_timer = nullptr;
}

void Page::setWaiting(bool waiting, const QString &label)
{
    if (waiting) {
        m_progressDialog->setLabelText(label);
        m_progressDialog->show();
        m_timer->start();
    } else {
        m_timer->stop();
        m_progressDialog->cancel();
    }
}

void Page::increaseBar()
{
    m_progressDialog->setValue(m_progressDialog->value() + 1);
}

void Page::showInfo(const QString &header, const QString &body)
{
    QMessageBox::information(this, header, body);
}

void Page::showWarning(const QString &header, const QString &body)
{
    QMessageBox::warning(this, header, body);
}

void Page::showError(const QString &header, const QString &body)
{
    QMessageBox::critical(this, header, body);
}
