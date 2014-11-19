/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QApplication>
#include <QMessageBox>
#include <QStyle>
#include <QDesktopWidget>
#include <QDebug>
#include <QPushButton>

#include "Page.h"

Page::Page(QWidget *parent) :
    QWidget(parent),
    m_progressDialog(nullptr)
{
    setWindowFlags(Qt::FramelessWindowHint);

    //setStyleSheet("background-color:rgb(240,240,240);");

    m_progressDialog = new QProgressDialog();
    m_progressDialog->setWindowModality(Qt::WindowModal);
    m_progressDialog->setModal(true);
    m_progressDialog->setWindowFlags(((m_progressDialog->windowFlags()
                                       | Qt::CustomizeWindowHint)
                                      & (~Qt::WindowCloseButtonHint | ~Qt::WindowMinMaxButtonsHint)));
    m_progressDialog->setCancelButtonText(tr("Abort"));
    m_progressDialog->setAutoClose(false);
    m_progressDialog->setRange(0, 0);
    //connect the aborting of the progress bar to a slot that will emit a signal
    //connect(m_progressDialog, SIGNAL(canceled()), this, SLOT(slotCancelProgressBar()));
}

Page::~Page()
{
    m_progressDialog->deleteLater();
    m_progressDialog = nullptr;
}

void Page::setWaiting(bool waiting, const QString &label)
{
    if (waiting) {
        m_progressDialog->setLabelText(label);
        m_progressDialog->show();
    } else {
        m_progressDialog->cancel();
    }
}

void Page::slotCancelProgressBar()
{
    m_progressDialog->cancel();
    emit signalDownloadCancelled();
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
