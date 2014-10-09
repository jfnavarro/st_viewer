/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef STARTPAGE_H
#define STARTPAGE_H

#include <memory>
#include "Page.h"

class QVBoxLayout;
class Error;
class AuthorizationManager;
class DataProxy;

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

    InitPage(QPointer<DataProxy> dataProxy,
             QPointer<AuthorizationManager> authManager,
             QWidget *parent = 0);
    virtual ~InitPage();

public slots:

    void onEnter() override;
    void onExit() override;

private slots:

    void slotAuthorizationError(QSharedPointer<Error> error);
    void slotLogOutButton();
    void slotAuthorized();

private:

    std::unique_ptr<Ui::InitPage> m_ui;

    //references to dataProxy and authManager
    QPointer<DataProxy> m_dataProxy;
    QPointer<AuthorizationManager> m_authManager;

    Q_DISABLE_COPY(InitPage);
};

#endif  // STARTPAGE_H //
