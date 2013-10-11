/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef SIMPLECRYPT_H
#define SIMPLECRYPT_H

#include <QByteArray>

class QIODevice;

// Simple stream based encryption class. Uses standard XOR approach to encrypt
// stream data with a unique key. Encodes data in variable length segments.
//TODO add crc32-based checksum to segment header for data validation.
class SimpleCrypt
{
public:
    static const quint64 EMPTY_KEY = Q_UINT64_C(0x00);

    enum ErrorCode
    {
        StreamOK = 0x00,
        StreamWriteError,
        StreamReadError,
        StreamPartialSegmentError,
        UnknownVersionError
    };

    SimpleCrypt();
    explicit SimpleCrypt(quint64 key);

    inline void setKey(quint64 key);
    inline quint64 getKey() const;
    inline bool hasKey() const;

    ErrorCode encodeStream(QIODevice *out) const;
    ErrorCode decodeStream(QIODevice *in);
    ErrorCode encodeSegment(QIODevice *out, const QByteArray &data) const;
    ErrorCode decodeSegment(QIODevice *in, QByteArray &data) const;

    enum Property
    {
        StreamHeaderSize,
        SegmentHeaderSize,
        LastErrorCode
    };
    int getProperty(Property code) const;

private:
    inline void encodeByteArray(QByteArray &data) const;
    inline void decodeByteArray(QByteArray &data) const;

    static inline quint8 randomSeed();

    // internal version control
    static const quint8 VERSION = 0x01;

    struct StreamHeader
    {
        quint8 version;
        quint8 flags;
    };
    struct SegmentHeader
    {
        quint16 size;
    };

    union {
        quint64 key64;
        quint8 key8[8];
    } m_key;
    mutable ErrorCode m_lastError;
};

/****************************************** DEFINITION ******************************************/

inline void SimpleCrypt::setKey(quint64 key) 
{ 
    m_key.key64 = key; 
}

inline quint64 SimpleCrypt::getKey() const 
{ 
    return m_key.key64; 
}

inline bool SimpleCrypt::hasKey() const 
{ 
    return m_key.key64 != EMPTY_KEY; 
}

inline void SimpleCrypt::encodeByteArray(QByteArray &data) const
{
    int size = data.size();
    int hist = 0;

    for (int i=0; i<size; ++i)
    {
        data[i] = data[i] ^ m_key.key8[i % 8] ^ hist;
        hist = data[i];
    }
}
inline void SimpleCrypt::decodeByteArray(QByteArray &data) const
{
    int size = data.size();
    int hist = 0;

    for (int i=0; i<size; ++i)
    {
        quint8 c = data[i];
        data[i] = c ^ hist ^ m_key.key8[i % 8];
        hist = c;
    }
}

inline quint8 SimpleCrypt::randomSeed()
{
    #if defined(DEBUG) || defined(_DEBUG)
        return 0;
    #else
        return static_cast<quint8>(qrand() & 0xFF);
    #endif
}

#endif // SimpleCrypt_H
