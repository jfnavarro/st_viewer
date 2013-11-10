/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QDebug>
#include "utils/DebugHelper.h"

#include "MathExtended.h"

namespace QtExt
{

const QSizeF clamp(const QSizeF& size, const QSizeF& min, const QSizeF& max, Qt::AspectRatioMode mode)
{
    if (mode == Qt::KeepAspectRatioByExpanding)
    {
        qDebug() <<
                    "[QtExt] const QSizeF clamp(const QSizeF&, const QSizeF&, constQSizeF&, Qt::AspectRatioMode)"
                    " does not distinguish between Qt::KeepAspectRatioByExpanding and Qt::KeepAspectRatio."
                    " Use Qt::KeepAspectRatio instead.";
    }

    QSizeF clampSize = size;
    if ( (clampSize.width() < min.width()) || (clampSize.height() < min.height()) )
    {
        if (mode == Qt::IgnoreAspectRatio)
        {
            clampSize = clampSize.expandedTo(min);
        }
        else
        {
            clampSize = clampSize.scaled(min, Qt::KeepAspectRatioByExpanding);
        }
    }
    if ( (clampSize.width() > max.width()) || (clampSize.height() > max.height()) )
    {
        if (mode == Qt::IgnoreAspectRatio)
        {
            clampSize = clampSize.boundedTo(max);
        }
        else
        {
            clampSize = clampSize.scaled(max, Qt::KeepAspectRatio);
        }
    }
    return clampSize;
}
}
