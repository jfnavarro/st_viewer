/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtMacExtras module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmacnativewidget.h"
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtGui/QWindow>
#include <QtGui/QGuiApplication>
#include <qpa/qplatformnativeinterface.h>
#endif
#include <qdebug.h>

#import <Cocoa/Cocoa.h>

/*!
    \class QMacNativeWidget
    \brief The QMacNativeWidget class provides a widget for Mac OS X that provides a way to put Qt widgets
    into Cocoa hierarchies.

    QMacNativeWidget bridges the gap between NSViews and QWidgets and makes it possible to put a
    hierarchy of Qt widgets into a non-Qt window or view.

    QMacNativeWidget pretends it is a window (i.e. isWindow() will return true),
    but it cannot be shown on its own. It needs to be put into a window
    when it is created or later through a native call.

    Note that QMacNativeWidget requires knowledge of Cocoa. All it
    does is get the Qt hierarchy into a window not owned by Qt. It is then up
    to the programmer to ensure it is placed correctly in the window and
    responds correctly to events.
*/

QT_BEGIN_NAMESPACE

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
NSView *getEmbeddableView(QWindow *qtWindow)
{
    // Make sure the platform window is created
    qtWindow->create();

    // Inform the window that it's a subwindow of a non-Qt window. This must be
    // done after create() because we need to have a QPlatformWindow instance.
    // The corresponding NSWindow will not be shown and can be deleted later.
    extern QPlatformNativeInterface::NativeResourceForIntegrationFunction resolvePlatformFunction(const QByteArray &functionName);
    typedef void (*SetEmbeddedInForeignViewFunction)(QPlatformWindow *window, bool embedded);
    reinterpret_cast<SetEmbeddedInForeignViewFunction>(resolvePlatformFunction("setEmbeddedInForeignView"))(qtWindow->handle(), true);

    // Get the Qt content NSView for the QWindow from the Qt platform plugin
    QPlatformNativeInterface *platformNativeInterface = QGuiApplication::platformNativeInterface();
    NSView *qtView = (NSView *)platformNativeInterface->nativeResourceForWindow("nsview", qtWindow);
    return qtView; // qtView is ready for use.
}
#endif

/*!
    Create a QMacNativeWidget with \a parentView as its "superview" (i.e.,
    parent). The \a parentView is  a NSView pointer.
*/
QMacNativeWidget::QMacNativeWidget(NSView *parentView)
    : QWidget(0)
{
    Q_UNUSED(parentView);

    //d_func()->topData()->embedded = true;
    setPalette(QPalette(Qt::transparent));
    setAttribute(Qt::WA_SetPalette, false);
    setAttribute(Qt::WA_LayoutUsesWidgetRect);
}

NSView *QMacNativeWidget::nativeView() const
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    winId();
    return getEmbeddableView(windowHandle());
#else
    return reinterpret_cast<NSView*>(winId());
#endif
}

/*!
    Destroy the QMacNativeWidget.
*/
QMacNativeWidget::~QMacNativeWidget()
{
}

/*!
    \reimp
*/
QSize QMacNativeWidget::sizeHint() const
{
    // QMacNativeWidget really does not have any other choice
    // than to fill its designated area.
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    if (windowHandle())
        return windowHandle()->size();
    return QWidget::sizeHint();
#else
    NSRect frame = [nativeView() frame];
    return QSize(frame.size.width, frame.size.height);
#endif
}
/*!
    \reimp
*/
bool QMacNativeWidget::event(QEvent *ev)
{
    return QWidget::event(ev);
}

QT_END_NAMESPACE
