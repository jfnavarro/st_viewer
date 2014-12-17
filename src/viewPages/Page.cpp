/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "Page.h"

#include <QApplication>
#include <QMessageBox>
#include <QStyle>
#include <QDesktopWidget>
#include <QDebug>
#include <QPushButton>

#include "data/DataProxy.h"

using namespace Globals;

Page::Page(QPointer<DataProxy> dataProxy, QWidget *parent) :
    QWidget(parent),
    m_dataProxy(dataProxy),
    m_progressDialog(nullptr)
{
    Q_ASSERT(!m_dataProxy.isNull());

    m_progressDialog = new QProgressDialog(nullptr);
    m_progressDialog->setWindowFlags(Qt::Tool | Qt::WindowTitleHint | Qt::WindowStaysOnTopHint
                                     | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    m_progressDialog->setStyleSheet("QProgressBar::chunk {background-color: rgb(0,155,60);} "
                                    + CELL_PAGE_SUB_MENU_BUTTON_STYLE +
                                    "QPushButton {width: 45px; height: 20px;} ");
    m_progressDialog->setWindowModality(Qt::WindowModal);
    m_progressDialog->setModal(true);
    m_progressDialog->setCancelButtonText(tr("Abort"));
    m_progressDialog->setAutoClose(false);
    m_progressDialog->setRange(0, 0);

    //connect the aborting of the progress bar to a slot that will emit a signal
    connect(m_progressDialog, SIGNAL(canceled()), this, SLOT(slotCancelProgressBar()));
}

Page::~Page()
{
    m_progressDialog->deleteLater();
    m_progressDialog = nullptr;
}

void Page::setWaiting(bool waiting, const QString &label)
{
    if (waiting) {
        m_progressDialog->move(
           window()->frameGeometry().topLeft() +
           window()->rect().center() - m_progressDialog->rect().center()
        );
        m_progressDialog->setLabelText(label);
        m_progressDialog->show();
    } else {
        m_progressDialog->cancel();
        m_progressDialog->close();
    }
}

void Page::slotCancelProgressBar()
{
    m_progressDialog->cancel();
    m_progressDialog->close();
    m_dataProxy->slotAbortActiveDownloads();
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
