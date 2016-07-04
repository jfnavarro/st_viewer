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

namespace
{

// Application flags must be set before instantiating QApplication
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
    const QString VERSION = QString("%1.%2.%3").arg(MAJOR).arg(MINOR).arg(PATCH);

    // Define some configuration flags
    setApplicationFlags();

    // Creates the application object
    QApplication app(argc, argv);
    app.setApplicationName(app.translate("main", "STViewer"));
    app.setApplicationVersion(VERSION);

    qDebug() << "Application started successfully.";

    // Install translation file
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

    // Create main window
    MainWindow mainWindow;
    app.setActiveWindow(&mainWindow);
    // Check for min requirements
    if (!mainWindow.checkSystemRequirements()) {
        qDebug() << "[Main] Error: Minimum requirements test failed!";
        return EXIT_FAILURE;
    }
    // Initialize graphic components
    mainWindow.init();
    // Show main window.
    mainWindow.show();
    // Authorize (if the online mode is supported)
    mainWindow.startAuthorization();
    // launch the app
    return app.exec();
}
