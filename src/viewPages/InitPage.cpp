/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "InitPage.h"

#include <QWidget>
#include <QDebug>

#include "error/Error.h"
#include "auth/AuthorizationManager.h"
#include "data/DataProxy.h"
#include "network/DownloadManager.h"

#include "ui_initpage.h"

InitPage::InitPage(QPointer<DataProxy> dataProxy,
                   QPointer<AuthorizationManager> authManager,
                   QWidget *parent) :
    Page(parent),
    m_ui(new Ui::InitPage()),
    m_dataProxy(dataProxy),
    m_authManager(authManager)
{
    Q_ASSERT(!m_dataProxy.isNull());
    Q_ASSERT(!m_authManager.isNull());

    m_ui->setupUi(this);

    m_ui->user_name->clear();
    m_ui->newExpButt->setEnabled(false);

    //connect signals for navigation
    connect(m_ui->newExpButt, SIGNAL(released()), this, SIGNAL(moveToNextPage()));
    connect(m_ui->logoutButt, SIGNAL(released()), this, SLOT(slotLogOutButton()));

    //connect authorization signals
    connect(m_authManager, SIGNAL(signalAuthorize()),
            this, SLOT(slotAuthorized()));
    connect(m_authManager, SIGNAL(signalError(QSharedPointer<Error>)),
            this, SLOT(slotAuthorizationError(QSharedPointer<Error>)));
}

InitPage::~InitPage()
{
}

void InitPage::onEnter()
{
    m_ui->newExpButt->clearFocus();
    m_ui->logoutButt->clearFocus();
}

void InitPage::onExit()
{
}

void InitPage::slotAuthorizationError(QSharedPointer<Error> error)
{
    //force clean access token and authorize again
    m_authManager->cleanAccesToken();
    m_authManager->startAuthorization();

    //not sure whether we want to show the error to the user or not when login failed
    qDebug() << "Error trying to log in " << error->name() << " " << error->description();
}

void InitPage::slotAuthorized()
{
    //clean the cache in the dataproxy
    m_dataProxy->clean();

    //load user from network
    setWaiting(true);
    async::DataRequest request = m_dataProxy->loadUser();
    setWaiting(false);

    if (request.isSuccessFul()) {
        const auto user = m_dataProxy->getUser();
        Q_ASSERT(!user.isNull());
        if (!user->enabled()) {
            showError(tr("Authorization Error"), tr("The current user is disabled"));
            return;
        }

        m_ui->user_name->setText(user->username());
        m_ui->newExpButt->setEnabled(true);
    } else {
        //TODO use the text present in request.getErrors()
        showError(tr("Authorization Error"), tr("Error loading the current user"));
    }
}

void InitPage::slotLogOutButton()
{
    //go to log in mode and force authorization
    m_ui->newExpButt->setEnabled(false);
    m_ui->user_name->clear();

    //force clean access token and authorize again
    m_authManager->cleanAccesToken();
    m_authManager->startAuthorization();
}
