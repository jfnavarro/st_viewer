#include <QDebug>
#include <QApplication>
#include <QMessageBox>
#include <QDir>
#include <QTranslator>
#include <QPixmap>
#include <QSplashScreen>
#include <QDesktopWidget>
#include <QFontDatabase>
#include <QSurfaceFormat>
#include <QScreen>

#include "mainWindow.h"
#include "options_cmake.h"

#include <iostream>
#include <omp.h>


// Application flags must be set before instantiating QApplication
void setApplicationFlags()
{

#ifdef Q_OS_MAC
    QApplication::setAttribute(Qt::AA_PluginApplication, false);
    QApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, false);
    // NOTE this is actually pretty important (be false)
    QApplication::setAttribute(Qt::AA_NativeWindows, false);
    // osx does not show icons on menus
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus, true);
#endif

    // unhandled mouse events will not be translated
    QApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents, false);
    QApplication::setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, false);
    // allows to create high-dpi pixmaps
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    // consistent font rendering
    QApplication::setAttribute(Qt::AA_Use96Dpi, true);
    // force usages of desktop opengl
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL, true);
}

int main(int argc, char **argv)
{
    const QString VERSION = QString("%1.%2.%3").arg(MAJOR).arg(MINOR).arg(PATCH);

    // Define some configuration flags
    setApplicationFlags();

    // Creates the application object
    QApplication app(argc, argv);
    app.setApplicationName(app.translate("main", "STViewer"));
    app.setApplicationVersion(VERSION);

    qDebug() << "Application started successfully.";

    // Create OpenGL context
    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    QSurfaceFormat::setDefaultFormat(format);

    // Set number of threads to the maximum possible
    omp_set_num_threads(omp_get_max_threads());

    // Create main window
    MainWindow mainWindow;
    app.setActiveWindow(&mainWindow);
    // Check for min requirements
    if (!mainWindow.checkSystemRequirements()) {
        qDebug() << "[Main] Error: Minimum requirements test failed!";
        QMessageBox::critical(nullptr,
                              app.tr("Error"),
                              app.tr("Minimum requirements not satisfied"));
        return EXIT_FAILURE;
    }
    // Initialize graphic components
    mainWindow.init();
    // Show main window.
    mainWindow.show();
    // launch the app
    const int return_code = app.exec();
    return return_code;
}
