/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef STARTPAGE_H
#define STARTPAGE_H

#include "Page.h"

#include "data/DataProxy.h"

class QVBoxLayout;
class Error;

namespace Ui {
class InitPage;
}

// this is the definition of the start page which contains logos,
//a login component and a start button
// the page as a stackwidget which will switch between login widget and logged widget
// as every page it implements the moveToNextPage and moveToPreviousPage
// the methods onEnter and onExit are called dynamically from the page manager.

//TODO authorizationManager should be passed as a reference here (currently obtaining it from DataProxy)
class InitPage : public Page
{
    Q_OBJECT

public:

    InitPage(QPointer<DataProxy> dataProxy, QWidget *parent = 0);
    virtual ~InitPage();

public slots:

    void onEnter() override;
    void onExit() override;

private slots:

    void slotAuthorizationError(QSharedPointer<Error> error);
    void slotLogOutButton();
    void slotAuthorized();

private:

    Ui::InitPage *m_ui;
    QPointer<DataProxy> m_dataProxy;

    Q_DISABLE_COPY(InitPage)
};

#endif  // STARTPAGE_H //
