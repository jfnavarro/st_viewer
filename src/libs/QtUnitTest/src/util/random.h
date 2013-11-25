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
inline const qint32 qrandi();
inline const qint32 qrandi(const qint32 min, const qint32 max);
inline const float qrandf();
inline const float qrandf(const float min, const float max);

} // namespace unit //

/****************************************** DEFINITION ******************************************/

namespace unit
{

inline const qint32 qrandi()
{
    return qrand();
}
inline const qint32 qrandi(const qint32 min, const qint32 max)
{
    if (min > max) {
        return qrandi(max, min);
    }

    return (qint32((max - min) * qrandf()) + min);
}
inline const float qrandf()
{
    return float(double(qrand()) / double(RAND_MAX));
}
inline const float qrandf(const float min, const float max)
{
    if (min > max) {
        return qrandf(max, min);
    }

    return (float((max - min) * qrandf()) + min);
}

} // namespace unit //

#endif // RANDOM_H //
