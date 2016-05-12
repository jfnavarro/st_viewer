#include <QDebug>
#include <QApplication>
#include <QMessageBox>
#include <QDir>
#include <QTranslator>
#include <QPixmap>
#include <QSplashScreen>
#include <QDesktopWidget>
#include <QFontDatabase>

#include "mainWindow.h"
#include "options_cmake.h"

#include <iostream>

static const QString VERSION = QString("%1.%2.%3").arg(MAJOR).arg(MINOR).arg(PATCH);

namespace
{

// application flags must be set before instantiating QApplication
void setApplicationFlags()
{

#ifdef Q_OS_MAC
    QApplication::setAttribute(Qt::AA_MacPluginApplication, false);
    QApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, true);
    // NOTE this is actually pretty important (be false)
    QApplication::setAttribute(Qt::AA_NativeWindows, false);
    // osx does not show icons on menus
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus, true);
#endif

    // unhandled mouse events will not be translated
    QApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents, false);
    QApplication::setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, false);
    // allows to create high-dpi pixmaps
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, false);
    // consistent font rendering
    QApplication::setAttribute(Qt::AA_Use96Dpi, false);
    // force usages of desktop opengl
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL, true);
}
}

int main(int argc, char **argv)
{
#ifdef Q_OS_LINUX
    // If this environment variable is not set we get (when having OpenGL resource leaks)
    // "Segmentation fault (core dumped)" when we exit
    // the application (running Ubuntu 14.04, Qt 5.3.1 with an nVidia graphics card)
    // The idea for this workaround came from here
    // http://www.opengl.org/discussion_boards/archive/index.php/t-173485.html
    setenv("__GL_NO_DSO_FINALIZER", "1", 1);
#endif

    setApplicationFlags();

    QApplication app(argc, argv);
    app.setApplicationName(app.translate("main", "STViewer"));
    app.setApplicationVersion(VERSION);

    qDebug() << "Application started successfully.";

// set library and plugins paths
// we need to tell the application where to look for plugins and resources
#if defined Q_OS_WIN
    app.addLibraryPath(QDir(app.applicationDirPath()).canonicalPath() + QDir::separator()
                       + "plugins");
#elif defined Q_OS_MAC
    QDir dir(QApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("PlugIns");
    app.addLibraryPath(dir.path());
#else
    QDir dir(QApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("plugins");
    app.addLibraryPath(dir.path());
#endif

    // install translation file
    bool initialized = true;
    QTranslator trans;
    initialized &= trans.load(":translations/locale_en_us.qm");
    initialized &= app.installTranslator(&trans);
    if (!initialized) {
        qDebug() << "[Main] Error: Unable to install the translations!";
        QMessageBox::critical(app.desktop()->screen(),
                              "Error",
                              app.tr("Unable to install the translations"));
        return EXIT_FAILURE;
    }

    // create mainWindow
    MainWindow mainWindow;
    app.setActiveWindow(&mainWindow);

    // check for min requirements
    if (!mainWindow.checkSystemRequirements()) {
        qDebug() << "[Main] Error: Minimum requirements test failed!";
        return EXIT_FAILURE;
    }
    // check wether the app is being built with internet support
    bool remote_data = false;
#ifdef REMOTE_DATA
    remote_data = true;
#endif
    // init graphic components
    mainWindow.init(remote_data);
    // show mainwindow.
    mainWindow.show();
    // authorize
    mainWindow.startAuthorization();
    // launch the app
    return app.exec();
}
