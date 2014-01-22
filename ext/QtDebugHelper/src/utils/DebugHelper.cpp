/******************************************************************************
 * This file is part of the Gluon Development Platform
 * Copyright (c) 2009 Dan Leinir Turthra Jensen <admin@leinir.d>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

// Adapted from code by wysota - http://blog.wysota.eu.org/index.php/2009/11/17/little-debugging-helper/

#include "DebugHelper.h"

#ifdef DBGHELPER_USES_PRINTF
#include <stdio.h>
#else
#include <QDebug>
#endif

using namespace GluonCore;

int DbgHelper::indent = 0;
int DbgHelper::colorIndex = 0;

static void DbgHelper_output(int color, int indent, const QString& prefix, const QString& funcName)
{
    QString text = QString(4 * indent, ' ') + QString(prefix + funcName);

    if (color >= 0) {
        text.prepend("\x1b[3" + QString::number(1 + color) + 'm');
        text.append("\x1b[39m");
    }

#ifndef DBGHELPER_USES_PRINTF
    qDebug() << text.toUtf8();
#else
    fprintf(stderr, "%s\n", qPrintable(text));
#endif
}

DbgHelper::DbgHelper()
{
    noFunctionName = true;

#ifdef NO_COLOR
    myColor = -1;
#else
    myColor = colorIndex;
    colorIndex = (colorIndex + 1) % 7;
#endif
}

DbgHelper::DbgHelper(const QString& t)
{
    noFunctionName = false;
    txt = t;

#ifdef NO_COLOR
    myColor = -1;
#else
    myColor = colorIndex;
    colorIndex = (colorIndex + 1) % 7;
#endif
    DbgHelper_output(myColor, indent, "BEGIN ", txt);

    ++indent;
}

void DbgHelper::addText(const QString& t)
{
    DbgHelper_output(myColor, indent, "", t);
}

DbgHelper::~DbgHelper()
{
    if (!noFunctionName) {
        --indent;
        DbgHelper_output(myColor, indent, "END   ", txt);
    }
}
