/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef SIMPLECRYPT_H
#define SIMPLECRYPT_H

#include <QByteArray>
#include <QSharedPointer>

class QIODevice;

// Simple stream based encryption class. Uses standard XOR approach to encrypt
// stream data with a unique key. Encodes data in variable length segments.
//TODO add crc32-based checksum to segment header for data validation.
class SimpleCrypt
{
    
public:
    
    static const quint64 EMPTY_KEY = Q_UINT64_C(0x00);

    enum ErrorCode {
        StreamOK = 0x00,
        StreamWriteError,
        StreamReadError,
        StreamPartialSegmentError,
        UnknownVersionError
    };

    enum Property {
        StreamHeaderSize,
        SegmentHeaderSize,
        LastErrorCode
    };
    
    SimpleCrypt();
    explicit SimpleCrypt(quint64 key);
    ~SimpleCrypt();
    
    void setKey(quint64 key);
    quint64 getKey() const;
    bool hasKey() const;

    ErrorCode encodeStream(QIODevice *out) const;
    ErrorCode decodeStream(QIODevice *in);
    ErrorCode encodeSegment(QIODevice *out,
                            const QByteArray &data) const;
    ErrorCode decodeSegment(QIODevice *in, QByteArray &data) const;

    int getProperty(Property code) const;

private:
    
    void encodeByteArray(QByteArray &data) const;
    void decodeByteArray(QByteArray &data) const;
    static quint8 randomSeed();
    
    // internal version control
    static const quint8 VERSION = 0x01;

    struct StreamHeader {
        quint8 version;
        quint8 flags;
    };
    
    struct SegmentHeader {
        quint16 size;
    };

    union {
        quint64 key64;
        quint8 key8[8];
    } m_key;

    mutable ErrorCode m_lastError;

    Q_DISABLE_COPY(SimpleCrypt)
};

#endif // SimpleCrypt_H
