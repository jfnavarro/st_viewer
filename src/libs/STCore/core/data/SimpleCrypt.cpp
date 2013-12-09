/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "SimpleCrypt.h"

#include <QDebug>
#include <QByteArray>
#include <QBuffer>

SimpleCrypt::SimpleCrypt() : m_lastError(StreamOK)
{
    setKey(EMPTY_KEY);
}

SimpleCrypt::SimpleCrypt(quint64 key) : m_lastError(StreamOK)
{
    setKey(key);
}

SimpleCrypt::ErrorCode SimpleCrypt::encodeStream(QIODevice *out) const
{
    // write stream header and abort on write error
    const StreamHeader header = { VERSION, 0x00 };
    if (out->write(reinterpret_cast<const char *>(&header), sizeof(header)) == -1) {
        return (m_lastError = StreamWriteError);
    }

    return StreamOK;
}

SimpleCrypt::ErrorCode SimpleCrypt::decodeStream(QIODevice *in)
{
    // read stream header and abort on read error
    StreamHeader header;
    if (in->peek(reinterpret_cast<char *>(&header), sizeof(header)) == -1) {
        return (m_lastError = StreamReadError);
    }
    // abort on invalid version
    if (header.version != VERSION) {
        return (m_lastError = UnknownVersionError);

    }
    // configure simple crypt
    //TODO

    // pop header from stream
    in->read(reinterpret_cast<char *>(&header), sizeof(header));
    return StreamOK;
}

SimpleCrypt::ErrorCode SimpleCrypt::encodeSegment(QIODevice *out, const QByteArray &data) const
{
    QByteArray encryptData;
    QBuffer encryptDataDevice(&encryptData);
    encryptDataDevice.open(QIODevice::WriteOnly);
    // append a random byte
    quint8 randomByte = randomSeed();
    encryptDataDevice.write(reinterpret_cast<char *>(&randomByte), sizeof(randomByte));
    // append integrity protection
    quint16 checksum = qChecksum(data.constData(), data.length());
    encryptDataDevice.write(reinterpret_cast<char *>(&checksum), sizeof(checksum));
    // append data
    encryptDataDevice.write(data);
    encryptDataDevice.close();
    // encrypt data
    encodeByteArray(encryptData);
    // write segment header and encrypted segment to output
    QByteArray outputData;
    QBuffer outputDataDevice(&outputData);
    outputDataDevice.open(QIODevice::WriteOnly);
    SegmentHeader segment = { static_cast<quint16>(encryptData.size()) };
    outputDataDevice.write(reinterpret_cast<char *>(&segment), sizeof(segment));
    outputDataDevice.write(encryptData);
    outputDataDevice.close();
    // abort on write error
    if (out->write(outputData) == -1) {
        return (m_lastError = StreamWriteError);
    }

    return StreamOK;
}

SimpleCrypt::ErrorCode SimpleCrypt::decodeSegment(QIODevice *in, QByteArray &data) const
{
    QByteArray decryptData;
    // read segment header and encrypted segment from input
    SegmentHeader segment;
    if (in->peek(reinterpret_cast<char *>(&segment), sizeof(SegmentHeader)) == -1) {
        return (m_lastError = StreamReadError);
    }
    // abort on partial segment
    if (in->bytesAvailable() < (static_cast<qint64>(sizeof(SegmentHeader)) + segment.size) ) {
        return (m_lastError = StreamPartialSegmentError);
    }
    in->read(reinterpret_cast<char *>(&segment), sizeof(SegmentHeader));
    decryptData = in->read(segment.size);
    // decrypt data
    decodeByteArray(decryptData);
    // check integrity protection
    //TODO implement

    // read data
    data = decryptData.mid(3);
    return StreamOK;
}

int SimpleCrypt::getProperty(Property code) const
{
    switch (code) {
    case StreamHeaderSize:
        return sizeof(StreamHeader);
        break;
    case SegmentHeaderSize:
        return sizeof(SegmentHeader) + sizeof(quint16);
        break;
    case LastErrorCode:
        return static_cast<int>(m_lastError);
        break;
    default:
        return -1;
        break;
    }
}

void SimpleCrypt::setKey(quint64 key)
{
    m_key.key64 = key;
}

quint64 SimpleCrypt::getKey() const
{
    return m_key.key64;
}

bool SimpleCrypt::hasKey() const
{
    return m_key.key64 != EMPTY_KEY;
}

void SimpleCrypt::encodeByteArray(QByteArray &data) const
{
    int size = data.size();
    int hist = 0;
    for (int i = 0; i < size; ++i) {
        data[i] = data[i] ^ m_key.key8[i % 8] ^ hist;
        hist = data[i];
    }
}
void SimpleCrypt::decodeByteArray(QByteArray &data) const
{
    int size = data.size();
    int hist = 0;

    for (int i = 0; i < size; ++i) {
        quint8 c = data[i];
        data[i] = c ^ hist ^ m_key.key8[i % 8];
        hist = c;
    }
}

quint8 SimpleCrypt::randomSeed()
{
#if defined(DEBUG) || defined(_DEBUG)
    return 0;
#else
    return static_cast<quint8>(qrand() & 0xFF);
#endif
}
