/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QBuffer>

#include "binarygenerator.h"

namespace unit
{

QByteArray BinaryGenerator::generate(qint64 maxSize) const
{
    QByteArray bytes(maxSize, Qt::Uninitialized);

    QBuffer bytesBuffer(&bytes);
    bytesBuffer.open(QIODevice::WriteOnly);

    while (!bytesBuffer.atEnd())
    {
        const int random = qrand();
        bytesBuffer.write(reinterpret_cast<const char *>(&random), sizeof(random));
    }

    bytesBuffer.close();

    return bytes;
}

} // namespace unit //
