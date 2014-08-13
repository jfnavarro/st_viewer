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
    m_animationLoading(false)
{
    m_progressDialog = new QProgressDialog(this);
    m_progressDialog->setWindowModality(Qt::WindowModal);
    m_progressDialog->setModal(true);
    m_progressDialog->setWindowFlags(((m_progressDialog->windowFlags()
                                       | Qt::CustomizeWindowHint)
                                      & (~Qt::WindowCloseButtonHint | ~Qt::WindowMinMaxButtonsHint)));
    m_progressDialog->setCancelButton(nullptr);
    m_progressDialog->setAutoClose(false);
    m_progressDialog->setRange(0,0);

    //TODO implement abort logic
    //connect(m_progressDialog, SIGNAL(canceled()), this, SLOT(cancelProgressBar()));
}

Page::~Page()
{
    m_progressDialog->deleteLater();
    m_progressDialog = nullptr;
}

void Page::setWaiting(bool waiting, const QString &label)
{
    //Q_UNUSED(waiting);
    Q_UNUSED(label);

    //TODO currently showing the QPogressDialog causes
    //to have the actions in the main menu bar disabled

    if (waiting) {
        QGuiApplication::setOverrideCursor(Qt::WaitCursor);
        //m_progressDialog->setLabelText(label);
        //m_progressDialog->show();
        m_animationLoading = true;
    } else {
        //m_progressDialog->cancel();
        //m_progressDialog->close();
        QGuiApplication::restoreOverrideCursor();
        m_animationLoading = false;
    }
}

bool Page::event(QEvent *event)
{
    if (m_animationLoading) {
        event->ignore();
        return true;
    }

    return QWidget::event(event);
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
