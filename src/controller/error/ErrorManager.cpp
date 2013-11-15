/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QDesktopWidget>
#include <QApplication>
#include "ErrorManager.h"

ErrorManager::ErrorManager(QObject* parent)
    : QObject(parent), m_parentContainer(0), m_errorDialog(0)
{

}

ErrorManager::~ErrorManager()
{

}

void ErrorManager::init(QWidget* parentContainer)
{
    // store reference to container to allow error manager to spawn child widgets
    m_parentContainer = parentContainer;
}

void ErrorManager::finalize()
{

}

void ErrorManager::slotHandleError(Error* error)
{
    // lazy initialization
    if (m_errorDialog.isNull())
    {
        m_errorDialog = new QErrorMessage(m_parentContainer);
        m_errorDialog->setModal(true);
        m_errorDialog->setWindowModality(Qt::ApplicationModal);
    }
    
    //TODO add proper tabulation of errors
    m_errorDialog->showMessage(error->name() + " : " + QString("\n") + error->description());
    m_errorDialog->raise();
    m_errorDialog->activateWindow();
    QRect scr = QApplication::desktop()->screenGeometry();
    m_errorDialog->move(scr.center() - m_errorDialog->rect().center());
    error->deleteLater(); //error is probably gonna be deleted as it is a child of the widget that launched it
}
