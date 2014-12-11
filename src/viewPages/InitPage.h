/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef STARTPAGE_H
#define STARTPAGE_H

#include "Page.h"

#include <memory>
#include "data/DataProxy.h"

class QVBoxLayout;
class Error;
class AuthorizationManager;
namespace Ui {
class InitPage;
}

// this is the definition of the start page which contains logos,
// a login component and a start button
// the page as a stackwidget which will switch between login widget and logged widget
// as every page it implements the moveToNextPage and moveToPreviousPage
// the methods onEnter and onExit are called dynamically from the page manager.
class InitPage : public Page
{
    Q_OBJECT

public:

    InitPage(QPointer<AuthorizationManager> authManager,
             QPointer<DataProxy> dataProxy,
             QWidget *parent = 0);
    virtual ~InitPage();

public slots:

    void onEnter() override;
    void onExit() override;

private slots:

    //when user clicks logs out (access token is cleared and controls are cleared as well)
    void slotLogOutButton();

    //slot to handle signals from the authorization manager
    void slotAuthorizationError(QSharedPointer<Error> error);
    void slotAuthorized();

    //used to be notified when the user has been downloaded from network
    //status contains the status of the operation (ok, abort, error)
    //type contain the type of download
    void slotDownloadFinished(const DataProxy::DownloadStatus status,
                              const DataProxy::DownloadType type);

private:

    std::unique_ptr<Ui::InitPage> m_ui;
    QPointer<AuthorizationManager> m_authManager;

    Q_DISABLE_COPY(InitPage)
};

#endif  // STARTPAGE_H //
