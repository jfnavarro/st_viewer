/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef RANDOM_H
#define RANDOM_H

#include <qglobal.h>

namespace unit
{

qint32 qrandi();
qint32 qrandi(const qint32 min, const qint32 max);
float qrandf();
float qrandf(const float min, const float max);

} // namespace unit //

/****************************************** DEFINITION ******************************************/

namespace unit
{

qint32 qrandi()
{
    return qrand();
}

qint32 qrandi(const qint32 min, const qint32 max)
{
    if (min > max) {
        return qrandi(max, min);
    }

    return (qint32((max - min) * qrandf()) + min);
}

float qrandf()
{
    return float(double(qrand()) / double(RAND_MAX));
}

float qrandf(const float min, const float max)
{
    if (min > max) {
        return qrandf(max, min);
    }

    return (float((max - min) * qrandf()) + min);
}

} // namespace unit //

#endif // RANDOM_H //
