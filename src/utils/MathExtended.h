/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/


#ifndef MATHEXTENDED_H
#define MATHEXTENDED_H

#include <QtCore/qmath.h>
#include <QSizeF>

namespace QtExt
{
// modulus for qreal types, (x % y)
inline qreal qMod(qreal x, qreal y);

// clamp size to
//NOTE: Qt::KeepAspectRation might be prone to numerical errors (ie. any skewing introduced due to num error will be kept)
const QSizeF clamp(const QSizeF& size, const QSizeF& min,
                   const QSizeF& max, Qt::AspectRatioMode mode = Qt::IgnoreAspectRatio);

} // namespace QtExt //

/******************************************************* DEFINITIONS *************************************************************************/

namespace QtExt
{

qreal qMod(qreal x, qreal y)
{
    return x - y * qFloor(x / y);
}

} // namespace QtExt //

#endif // MATHEXTENDED_H //
