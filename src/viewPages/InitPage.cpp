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

InitPage::InitPage(QWidget *parent) : Page(parent)
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
    connect(ui->newExpButt, SIGNAL(released()), this, SLOT(slotLoadData()));
    connect(ui->logoutButt, SIGNAL(released()), this, SLOT(slotLogOutButton()));
    
    AuthorizationManager* authorizationManager = AuthorizationManager::getInstance();
    connect(authorizationManager, SIGNAL(signalAuthorize()), this, SLOT(slotAuthorized()));
    connect(authorizationManager, SIGNAL(signalError(Error*)), this, SLOT(slotAuthorizationError(Error*)));
    authorizationManager->start();  //start the authorization (NOTE move outside of constructor??)
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

void InitPage::slotNetworkError(Error *error)
{
    setWaiting(false);
    emit signalError(error);
}

void InitPage::slotAuthorized()
{
    //I have been authorized, clean data proxy and load
    DataProxy *dataProxy = DataProxy::getInstance();
    dataProxy->clean(); //clean the cache
    
    const async::DataRequest* request = dataProxy->loadUser();
    Q_ASSERT_X(request, "InitPage", "DataRequest object is null");
    
    if (request->return_code() == async::DataRequest::CodeError) {
        qDebug() << "[InitPage] Error: loading user";
        Error *error = new Error("Authorization Error", "Error loading the current user.");
        emit signalError(error); 
    } else if (request->return_code() == async::DataRequest::CodePresent) {
        DataProxy::UserPtr user = dataProxy->getUser();
        ui->user_name->setText(user.data()->username());
        ui->newExpButt->setEnabled(true);     
    } else {
        connect(request, SIGNAL(signalFinished()), this, SLOT(slotUserLoaded()));
        connect(request, SIGNAL(signalError(Error*)), this, SLOT(slotNetworkError(Error*)));
        setWaiting(true);
    }
}

void InitPage::slotUserLoaded()
{
    const async::DataRequest *request =
            reinterpret_cast<const async::DataRequest*>(sender());
    Q_ASSERT_X(request, "InitPage", "DataRequest object is null");
    
    setWaiting(false);
    
    if (request->return_code() == async::DataRequest::CodeSuccess) { //ignore when abort/timedout or error
        //User has been loaded succesfully, go to logged mode
        DataProxy::UserPtr user = DataProxy::getInstance()->getUser();
        ui->user_name->setText(user.data()->username());
        ui->newExpButt->setEnabled(true);
    }
}

void InitPage::slotLoadData()
{
    const async::DataRequest* request = DataProxy::getInstance()->loadDatasets();
    Q_ASSERT_X(request, "InitPage", "DataRequest object is null");
    
    if (request->return_code() == async::DataRequest::CodeError) {
        qDebug() << "[InitPage] Error: loading datasets";
        Error *error = new Error("Data Error", "Error loading the datasets.");
        emit signalError(error);  
    } else if (request->return_code() == async::DataRequest::CodePresent) {      
        emit moveToNextPage();      
    } else {
        connect(request, SIGNAL(signalFinished()), this, SLOT(slotDataLoaded()));
        connect(request, SIGNAL(signalError(Error*)), this, SLOT(slotNetworkError(Error*)));
        setWaiting(true);   
    }
}

void InitPage::slotDataLoaded()
{
    async::DataRequest *request = reinterpret_cast<async::DataRequest*>(sender());
    Q_ASSERT_X(request, "InitPage", "DataRequest object is null");
    setWaiting(false);
    if (request->return_code() == async::DataRequest::CodeSuccess) {       
        emit moveToNextPage();     
    }
}

void InitPage::slotLogOutButton()
{
    //go to log in mode and force authorization
    ui->newExpButt->setEnabled(false);
    ui->user_name->setText("");
    
    AuthorizationManager *auth = AuthorizationManager::getInstance();
    auth->cleanAccesToken(); //force clean access token
    auth->forceAuthentication(); //authorize again
}

void InitPage::setWaiting(bool waiting)
{
    ui->logoutButt->setEnabled(!waiting);
    ui->newExpButt->setEnabled(!waiting);
    Page::setWaiting(waiting);
}
