/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef CRYPTDEVICE_H
#define CRYPTDEVICE_H

#include <QIODevice>
#include <QSharedPointer>

#include "SimpleCrypt.h"

// Extend the simple crypt class by providing io device version. Simple crypt
// device will plug in to the Qt device interface and allow easy encryption of
// data streams. The buffer size determines the size of encoded stream
// segments.
class SimpleCryptDevice : public QIODevice
{

public:
    
    SimpleCryptDevice(QSharedPointer<QIODevice> device, QObject *parent = 0);
    SimpleCryptDevice(QSharedPointer<QIODevice>, quint64 key, QObject *parent = 0);
    virtual ~SimpleCryptDevice();
    
    // clear the buffer
    void flush();
    
    virtual qint64 bytesAvailable() const;
    virtual qint64 bytesToWrite() const;
    virtual void close();
    virtual bool isSequential() const;
    virtual bool open(OpenMode mode);
    virtual qint64 pos() const;
    virtual bool seek(qint64 pos);
    virtual qint64 size() const;

protected:
    
    virtual qint64 readData(char *data, qint64 maxSize);
    virtual qint64 writeData(const char *data, qint64 maxSize);

private:
    
    qint64 readBuffer(char *out, qint64 maxSize);
    qint64 writeBuffer(const char *out, qint64 maxSize);

    SimpleCrypt m_crypt;
    QSharedPointer<QIODevice> m_device;
    QByteArray m_buffer;

    Q_DISABLE_COPY(SimpleCryptDevice)
};

#endif // CRYPTDEVICE_H //
