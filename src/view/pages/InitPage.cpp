/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QWidget>
#include <QDebug>
#include "utils/DebugHelper.h"
#include "controller/error/Error.h"
#include "controller/auth/AuthorizationManager.h"
#include "ui_initpage.h"
#include "InitPage.h"

InitPage::InitPage(QWidget *parent)
    : QStackedWidget(parent)
{
    onInit();
}

InitPage::~InitPage()
{
    //NOTE no neeed to delete m_startWidget and m_loginWidget since they are children of this class
}

void InitPage::onInit()
{
    DEBUG_FUNC_NAME
    
    //create the start widget
    m_startWidget = new Ui::InitPage(); 
    m_startWidget->setupUi(this);
    
    //create login widget
    m_loginWidget = new QWidget(this);
    
    //add widgets to the page
    addWidget(m_startWidget->widget);
    addWidget(m_loginWidget);
    
    //current is the login by default
    setCurrentWidget(m_loginWidget);
    m_startWidget->user_name->setText("");
    m_startWidget->newExpButt->setEnabled(false);
    
    //connect signals
    connect(m_startWidget->newExpButt, SIGNAL(released()), this, SLOT(slotLoadData()));
    connect(m_startWidget->logoutButt, SIGNAL(released()), this, SLOT(slotLogOutButton()));
    
    AuthorizationManager* authorizationManager = AuthorizationManager::getInstance();
    connect(authorizationManager, SIGNAL(signalAuthorize()), this, SLOT(slotAuthorized()));
    connect(authorizationManager, SIGNAL(signalError(Error*)), this, SLOT(slotAuthorizationError(Error*)));
    connect(authorizationManager, SIGNAL(signalLoginAborted()), this, SLOT(slotAuthorizationExit()));
    
    authorizationManager->start(m_loginWidget);  //start the authorization
}

void InitPage::onEnter()
{
    DEBUG_FUNC_NAME
    m_startWidget->newExpButt->clearFocus();
    m_startWidget->logoutButt->clearFocus();
}

void InitPage::onExit()
{
    DEBUG_FUNC_NAME

}

void InitPage::slotAuthorizationError(Error *error)
{
    
    // there was an error authorizing so we force log in or switch to start page??
    
    //m_startWidget->user_name->setText("");
    //m_startWidget->newExpButt->setEnabled(false);
    //setCurrentWidget(m_startWidget->widget);
    
    AuthorizationManager *auth = AuthorizationManager::getInstance();
    auth->cleanAccesToken(); //force clean access token
    auth->forceAuthentication(); //authorize again
    setCurrentWidget(m_loginWidget);   
    emit signalError(error);
}

void InitPage::slotNetworkError(Error *error)
{
    setWaiting(false);
    emit signalError(error);
}

void InitPage::slotAuthorized()
{   
    //I have been authorized, clean data proxy and load
    
    //NOTE user no need for this when dataproxy is completed
    DataProxy *dataProxy = DataProxy::getInstance();
    dataProxy->clean(); //clean the cache
    
    async::DataRequest* request = dataProxy->loadUser();
    Q_ASSERT_X(request, "InitPage", "DataRequest object is null");
    
    if(request->return_code() == async::DataRequest::CodeError)
    {
        qDebug() << "[InitPage] Error: loading user";
        Error *error = new Error("Authorization Error", "Error loading the current user.");
        emit signalError(error);
    }
    else if(request->return_code() == async::DataRequest::CodePresent)
    {
        DataProxy::UserPtr user = DataProxy::getInstance()->getUser();
        m_startWidget->user_name->setText(user.data()->username());
        m_startWidget->newExpButt->setEnabled(true);
    }
    else
    {
        connect(request, SIGNAL(signalFinished()), this, SLOT(slotUserLoaded()));
        connect(request, SIGNAL(signalError(Error*)), this, SLOT(slotNetworkError(Error*)));
        setWaiting(true);
    }
    
    setCurrentWidget(m_startWidget->widget);
}

void InitPage::slotAuthorizationExit()
{
    //user clicked exit on login widget
    setCurrentWidget(m_startWidget->widget);
}

void InitPage::slotUserLoaded()
{
    async::DataRequest *request = reinterpret_cast<async::DataRequest*>(sender());
    Q_ASSERT_X(request, "InitPage", "DataRequest object is null");
    
    setWaiting(false);
    
    if(request->return_code() == async::DataRequest::CodeSuccess) //ignore when abort/timedout or error
    {
        //User has been loaded succesfully, go to logged mode
        DataProxy::UserPtr user = DataProxy::getInstance()->getUser();
        m_startWidget->user_name->setText(user.data()->username());
        m_startWidget->newExpButt->setEnabled(true);
    }
}

void InitPage::slotLoadData()
{
    async::DataRequest* request = DataProxy::getInstance()->loadDatasets();
    Q_ASSERT_X(request, "InitPage", "DataRequest object is null");
    
    if(request->return_code() == async::DataRequest::CodeError)
    {
        qDebug() << "[InitPage] Error: loading datasets";
        Error *error = new Error("Data Error", "Error loading the datasets.");
        emit signalError(error);
    }
    else if(request->return_code() == async::DataRequest::CodePresent)
    {
        emit moveToNextPage();
    }
    else
    {
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
    
    if(request->return_code() == async::DataRequest::CodeSuccess) //ignore when abort/timedout or error
    {
        emit moveToNextPage();
    }
}

void InitPage::slotLogOutButton()
{
    //go to log in mode and force authorization
    m_startWidget->newExpButt->setEnabled(false);
    m_startWidget->user_name->setText("");
    AuthorizationManager *auth = AuthorizationManager::getInstance();
    auth->cleanAccesToken(); //force clean access token
    auth->forceAuthentication(); //authorize again
    setCurrentWidget(m_loginWidget);
}

void InitPage::setWaiting(bool waiting)
{
    m_startWidget->logoutButt->setEnabled(!waiting);
    m_startWidget->newExpButt->setEnabled(!waiting);
    
    if(waiting)
    {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    }
    else
    {
        QApplication::restoreOverrideCursor();
        QApplication::processEvents();
    }
    
}
