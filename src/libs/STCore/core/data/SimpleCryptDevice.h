/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef CRYPTDEVICE_H
#define CRYPTDEVICE_H

#include <QIODevice>

#include "SimpleCrypt.h"

// Extend the simple crypt class by providing io device version. Simple crypt
// device will plug in to the Qt device interface and allow easy encryption of
// data streams. The buffer size determines the size of encoded stream
// segments.
class SimpleCryptDevice : public QIODevice
{

public:
    
    explicit SimpleCryptDevice(QIODevice *device, QObject *parent = 0);
    explicit SimpleCryptDevice(QIODevice *device, quint64 key, QObject *parent = 0);
    virtual ~SimpleCryptDevice();
    
    // clear the bugger
    void flush();
    
    virtual qint64 bytesAvailable() const;
    virtual qint64 bytesToWrite() const;
    virtual void close();

    virtual inline bool isSequential() const { return true; }

    virtual bool open(OpenMode mode);

    virtual inline qint64 pos() const { return m_device->pos(); }

    virtual inline bool seek(qint64 pos)
    {
        Q_UNUSED(pos);
        //TODO implement
        return false;
    }
    
    virtual qint64 size() const { return m_device->size(); }

protected:
    
    virtual qint64 readData(char *data, qint64 maxSize);
    virtual qint64 writeData(const char *data, qint64 maxSize);

private:
    
    qint64 readBuffer(char *out, qint64 maxSize);
    qint64 writeBuffer(const char *out, qint64 maxSize);

    static const qint64 DEFAULT_BUFFER_SIZE;
    
    SimpleCrypt m_crypt;
    QIODevice *m_device;
    QByteArray m_buffer;
};

#endif // CRYPTDEVICE_H //
