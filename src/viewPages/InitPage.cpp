/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "InitPage.h"

#include <QWidget>
#include <QDebug>
#include "utils/DebugHelper.h"

#include "error/Error.h"
#include "auth/AuthorizationManager.h"

#include "network/DownloadManager.h"

#include "data/DataProxy.h"

#include "ui_initpage.h"

InitPage::InitPage(QWidget *parent) :
    Page(parent)
{
    onInit();
}

InitPage::~InitPage()
{
    delete ui;
}

void InitPage::onInit()
{
    DEBUG_FUNC_NAME

    //create the start widget
    ui = new Ui::InitPage;
    ui->setupUi(this);
    ui->user_name->setText("");
    ui->newExpButt->setEnabled(false);

    //connect signals
    connect(ui->newExpButt, SIGNAL(released()), this, SIGNAL(moveToNextPage()));
    connect(ui->logoutButt, SIGNAL(released()), this, SLOT(slotLogOutButton()));
    
    AuthorizationManager* authorizationManager = AuthorizationManager::getInstance();
    connect(authorizationManager, SIGNAL(signalAuthorize()), this, SLOT(slotAuthorized()));
    connect(authorizationManager, SIGNAL(signalError(Error*)), this, SLOT(slotAuthorizationError(Error*)));
    authorizationManager->start();
}

void InitPage::onEnter()
{
    DEBUG_FUNC_NAME
    ui->newExpButt->clearFocus();
    ui->logoutButt->clearFocus();
}

void InitPage::onExit()
{
    DEBUG_FUNC_NAME
}

void InitPage::slotAuthorizationError(Error *error)
{
    Q_UNUSED(error);
    AuthorizationManager *auth = AuthorizationManager::getInstance();
    auth->cleanAccesToken(); //force clean access token
    auth->forceAuthentication(); //authorize again
    //emit signalError(error); //NOTE do we want to emit an error for this?
}

void InitPage::slotAuthorized()
{
    //clean the cache
    DataProxy *dataProxy = DataProxy::getInstance();
    dataProxy->clean();

    //load user from network (synchronous)
    setWaiting(true);
    async::DataRequest request = dataProxy->loadUser();
    setWaiting(false);

   if (request.return_code() == async::DataRequest::CodePresent
               || request.return_code() == async::DataRequest::CodeSuccess) {
        const auto user = dataProxy->getUser();
        ui->user_name->setText(user.data()->username());
        ui->newExpButt->setEnabled(true);
    } else {
       Error *error = new Error("Authorization Error", "Error loading the current user.", this);
       emit signalError(error);
    }
}

void InitPage::slotLogOutButton()
{
    //go to log in mode and force authorization
    ui->newExpButt->setEnabled(false);
    ui->user_name->setText("");
    
    AuthorizationManager *auth = AuthorizationManager::getInstance();
    //force clean access token and authorize again
    auth->cleanAccesToken();
    auth->forceAuthentication();
}

void InitPage::setWaiting(bool waiting)
{
    ui->logoutButt->setEnabled(!waiting);
    ui->newExpButt->setEnabled(!waiting);
    Page::setWaiting(waiting);
}
