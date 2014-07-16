/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef CRYPTDEVICE_H
#define CRYPTDEVICE_H

#include <memory>
#include <QIODevice>

#include "SimpleCrypt.h"

// Extend the simple crypt class by providing io device version. Simple crypt
// device will plug in to the Qt device interface and allow easy encryption of
// data streams. The buffer size determines the size of encoded stream
// segments.
class SimpleCryptDevice : public QIODevice
{

public:

    typedef std::unique_ptr<QIODevice> resourceDeviceType;

    SimpleCryptDevice(resourceDeviceType device, QObject *parent = 0);
    SimpleCryptDevice(resourceDeviceType device, quint64 key, QObject *parent = 0);

    virtual ~SimpleCryptDevice();
    
    // clear the buffer
    void flush();
    
    qint64 bytesAvailable() const override;
    qint64 bytesToWrite() const override;
    void close() override;
    bool isSequential() const override;
    bool open(OpenMode mode) override;
    qint64 pos() const override;
    bool seek(qint64 pos) override;
    qint64 size() const override;

protected:
    
    qint64 readData(char *data, qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 maxSize) override;

private:
    
    qint64 readBuffer(char *out, qint64 maxSize);
    qint64 writeBuffer(const char *out, qint64 maxSize);

    SimpleCrypt m_crypt;
    resourceDeviceType m_device;
    QByteArray m_buffer;

    Q_DISABLE_COPY(SimpleCryptDevice)
};

#endif // CRYPTDEVICE_H //
