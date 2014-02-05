/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QBuffer>

#include "asciigenerator.h"

namespace unit
{

QByteArray AsciiGenerator::generate(qint64 maxSize) const
{
    QByteArray bytes(maxSize, Qt::Uninitialized);

    QBuffer bytesBuffer(&bytes);
    bytesBuffer.open(QIODevice::WriteOnly);

    while (!bytesBuffer.atEnd()) {
        const qreal random = ( static_cast<qreal>(qrand()) / static_cast<qreal>(RAND_MAX) );
        const char ascii = (static_cast<char>((0x80 - 0x20) * random) + 0x20) & 0x7F;
        bytesBuffer.write(reinterpret_cast<const char *>(&ascii), sizeof(ascii));
    }

    bytesBuffer.close();

    return bytes;
}

} // namespace unit //
