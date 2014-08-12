/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QDebug>
#include <QtSingleApplication>
#include <QMessageBox>
#include <QDir>
#include <QTranslator>

#if defined Q_OS_LINUX
    #include <QStyleFactory>
#endif

#include <iostream>

#include "utils/Utils.h"
#include "stVi.h"

#include "options_cmake.h"

namespace {

void setLocalPaths(QtSingleApplication *app)
{
    // we need to tell the application where to look for plugins and resources
#if defined Q_OS_WIN
    app->addLibraryPath(QDir(app->applicationDirPath()).canonicalPath()
                        + QDir::separator() + "plugins");
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

bool installTranslator(QtSingleApplication *app)
{
    //install translation file
    bool initialized = true;
    QTranslator trans;
    initialized &= trans.load(TRANSLATION_FILE, ":/translations");
    initialized &= app->installTranslator(&trans);
    return initialized;
}

//application flags must be set before instantiating QApplication
void setApplicationFlags()
{

#if defined Q_OS_LINUX
    QApplication::setStyle("Windows");
#endif

#ifdef Q_OS_MAC
    QApplication::setAttribute(Qt::AA_MacPluginApplication, false);
    QApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, true);
    //NOTE this is actually pretty important (be false)
    QApplication::setAttribute(Qt::AA_NativeWindows, false);
    //osx does not show icons on menus
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus, true);
#endif

    //unhandled mouse events will not be translated
    QApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents, false);
    QApplication::setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, false);
    //allows to create high-dpi pixmaps
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, false);
    //consistent font rendering
    QApplication::setAttribute(Qt::AA_Use96Dpi, false);
    //force usages of desktop opengl
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL, true);
}

}

int main(int argc, char** argv)
{
    setApplicationFlags();

    QtSingleApplication *app = new QtSingleApplication(argc, argv);
    app->setApplicationName(app->translate("main", "stVi"));
    app->setOrganizationName("Spatial Transcriptomics AB");
    app->setOrganizationDomain("spatialtranscriptomics.com");
    app->setApplicationVersion(Globals::VERSION);

    if (app->isRunning()) {
        app->sendMessage(app->tr("Another instance of stVi is already open"));
        delete app;
        return EXIT_FAILURE;
    } else {
        qDebug() << "Application started successfully.";
    }

    setLocalPaths(app);
    if (!installTranslator(app)) {
        qDebug() << "[Main] Error: Unable to install the translations!";
        QMessageBox::critical(nullptr, "Error",
                              app->tr("Unable to install the translations"));
        delete app;
        return EXIT_FAILURE;
    }

    //create mainWindow
    stVi *mainWindow = new stVi();
    app->setActivationWindow(mainWindow);

    // connect message queue to the main window.
    QObject::connect(app, SIGNAL(messageReceived(QString, QObject *)),
                     mainWindow, SLOT(handleMessage(QString)));

    //check for min requirements
    if (!mainWindow->checkSystemRequirements()) {
        delete mainWindow;
        return EXIT_FAILURE;
    }

    //init graphic components
    mainWindow->init();
    // show mainwindow.
    mainWindow->show();
    // launch the app
    int res = app->exec();
    delete mainWindow;
    qDebug() << "Application closed successfully.";
    return res;
}
