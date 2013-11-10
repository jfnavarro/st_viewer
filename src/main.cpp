/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QDebug>
#include "utils/DebugHelper.h"

#include <QtSingleApplication>
#include <QMessageBox>
#include <QDir>
#include <QTranslator>

#include <iostream>

#include "utils/Utils.h"
#include "controller/stVi.h"

#include "options_cmake.h"

static void setLocalPaths(QtSingleApplication *app)
{
    // we need to tell the application where to look for plugins and resources
#if defined Q_OS_WIN
    
    app->addLibraryPath( QDir(app->applicationDirPath()).canonicalPath()
                         +QDir::separator()+"plugins");

#elif defined Q_OS_MAC
    
    QDir dir(QApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("PlugIns");
    app->addLibraryPath(dir.path());
    
#else

    QDir dir(QApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("plugins");
    app->addLibraryPath(dir.path());
    
#endif
}

int main(int argc, char** argv)
{
    try
    {
        QtSingleApplication *app = new QtSingleApplication(argc, argv);
        app->setApplicationName(app->translate("main", "stVi"));
        app->setOrganizationName("Spatial Transcriptomics AB");
        app->setOrganizationDomain("spatialtranscriptomics.com");
        app->setApplicationVersion(Globals::VERSION);
        
        if (app->isRunning())
        {
            app->sendMessage("Another instance of stVi is already open");
            delete app;
            return 0;
        }
        else
        {
            qDebug() << "Application started successfully.";
        }

        setLocalPaths(app);
        
        //install translation file

        bool initialized = true;
        /* translator */
        QTranslator trans;
        QString trans_name = QString(TRANSLATION_FILE);
        QLocale locale = QLocale::system();
        
        qDebug() << "[Main] Local language : " << locale.name().toLower();
        
#if defined Q_OS_MAC
        
        QString trans_dir = app->applicationDirPath() + "/../Resources/translations/";

#elif defined Q_OS_WIN

        QString trans_dir = QDir(app->applicationDirPath()).canonicalPath()
                +QDir::separator()+"translations"+QDir::separator();

#else

        QString trans_dir = app->applicationDirPath() + QString(TRANSLATIONS_DIR);

#endif

        qDebug() << "[Main] languages location : " << trans_dir << " file name = " << trans_name;
        
        initialized &= trans.load(trans_name, trans_dir);
        initialized &= app->installTranslator(&trans);
        
        if (!initialized)
        {
            qDebug() << "[Main] Error: Unable to install the translations!";
            QMessageBox::information(0, "Error",
                                     "Unable to install the translations");
            delete app;
            return 0;
        }
        
        //create mainWindow
        stVi *mainWindow = new stVi();
        app->setActivationWindow(mainWindow);
        // connect message queue to the main window.
        QObject::connect(app, SIGNAL(messageReceived(QString,QObject *)),mainWindow, SLOT(handleMessage(QString)));
        //check for min requirements
        if(!mainWindow->checkSystemRequirements())
        {
            delete mainWindow;
            //delete app;
            return 0;
        }
        //init graphic components
        mainWindow->init();
        // show mainwindow.
        mainWindow->show();
        // launch the app
        int res = app->exec();
        delete mainWindow;
        //delete app;
        qDebug() << "Application closed successfully.";
        return res;
    }
    catch(const std::exception &e)
    {
        std::cerr << "Exception Error : " << e.what() << std::endl;
        return 0;
    }
    catch(...)
    {
        std::cerr << "Unknown error, contact the developers " << std::endl;
        return 0;
    }
}
