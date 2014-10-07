/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef WRAPPEDFILEDTO_H
#define WRAPPEDFILEDTO_H

#include <QObject>

#include "zlib.h"

#define GZIP_WINDOWS_BIT MAX_WBITS + 16
#define GZIP_CHUNK_SIZE 32 * 1024

// DTOs (Data transform objects) provides the bridge between the client
// specific data model objects and the server data. This is accomplished by
// defining dynamic properties that enable automated serialization and
// deserialization of server data.

// DTO used to parse the endpoint S3Resource that is meant to encapsulate
// file resources. The file is encoded with 64 bits as a byte array and compressed
// with zlib

//TODO move declarations to CPP and/or consider removing DTOs
class WrappedFileDTO : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString contentType READ contentType WRITE contentType)
    Q_PROPERTY(QString contentEncoding READ contentEncoding WRITE contentEncoding)
    Q_PROPERTY(QString filename READ fileName WRITE fileName)
    Q_PROPERTY(QByteArray file READ compressedFile WRITE compressedFile)
    Q_PROPERTY(qlonglong size READ size WRITE size)

public:

    explicit WrappedFileDTO(QObject* parent = 0) : QObject(parent) { }
    ~WrappedFileDTO() { }

    // binding
    const QString contentType() const { return m_contentType; }
    const QString contentEncoding() const { return m_contentEncoding; }
    const QString fileName() const { return m_fileName; }
    const QByteArray compressedFile() const { return m_rawCompressedFile; }
    qlonglong size() const { return m_size; }

    // getters
    void contentType(const QString& contentType) { m_contentType = contentType; }
    void contentEncoding(const QString& contentEncoding) { m_contentEncoding = contentEncoding; }
    void fileName(const QString& fileName) { m_fileName = fileName; }
    void compressedFile(const QByteArray& file) { m_rawCompressedFile = file; }
    void size(const qlonglong size) { m_size = size; }

    // uncompress a GZIP byte array encoded base 64
    // and returns uncompress byte array or empty if something went wrong
    const QByteArray decompressedFile() const
    {
        // input array from the server is encoded base 64
        QByteArray input = QByteArray::fromBase64(m_rawCompressedFile);

        // Prepare output
        QByteArray output;

        // Is there something to do?
        if(input.length() > 0)
        {
            // Prepare inflater status
            z_stream strm;
            strm.zalloc = Z_NULL;
            strm.zfree = Z_NULL;
            strm.opaque = Z_NULL;
            strm.avail_in = 0;
            strm.next_in = Z_NULL;

            // Initialize inflater
            int ret = inflateInit2(&strm, GZIP_WINDOWS_BIT);

            if (ret != Z_OK) {
                qDebug() << "[WrappedFileDTO] can't uncompress file. Error decoding file";
                return QByteArray();
            }

            // Extract pointer to input data
            char *input_data = input.data();
            int input_data_left = input.length();

            // Decompress data until available
            do {
                // Determine current chunk size
                int chunk_size = qMin(GZIP_CHUNK_SIZE, input_data_left);

                // Check for termination
                if(chunk_size <= 0) {
                    break;
                }

                // Set inflater references
                strm.next_in = (unsigned char*)input_data;
                strm.avail_in = chunk_size;

                // Update interval variables
                input_data += chunk_size;
                input_data_left -= chunk_size;

                // Inflate chunk and cumulate output
                do {

                    // Declare vars
                    char out[GZIP_CHUNK_SIZE];

                    // Set inflater references
                    strm.next_out = (unsigned char*)out;
                    strm.avail_out = GZIP_CHUNK_SIZE;

                    // Try to inflate chunk
                    ret = inflate(&strm, Z_NO_FLUSH);

                    switch (ret) {
                    case Z_NEED_DICT:
                        ret = Z_DATA_ERROR;
                    case Z_DATA_ERROR:
                    case Z_MEM_ERROR:
                    case Z_STREAM_ERROR:
                        qDebug() << "[WrappedFileDTO] can't uncompress file. Streaming error";
                        // Clean-up
                        inflateEnd(&strm);
                        return QByteArray();
                    }

                    // Determine decompressed size
                    int have = (GZIP_CHUNK_SIZE - strm.avail_out);

                    // Cumulate result
                    if(have > 0) {
                        output.append((char*)out, have);
                    }

                } while (strm.avail_out == 0);

            } while (ret != Z_STREAM_END);

            // Clean-up
            inflateEnd(&strm);

            // Return
            if (ret == Z_STREAM_END) {
                return output;
            }
        }

        return output;
    }

private:

    QString m_contentType;
    QString m_contentEncoding;
    QString m_fileName;
    QByteArray m_rawCompressedFile;
    qlonglong m_size;
};

#endif // WRAPPEDFILEDTO_H
