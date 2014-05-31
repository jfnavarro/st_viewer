/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "SimpleCryptDevice.h"

#include <QBuffer>

static const qint64 DEFAULT_BUFFER_SIZE = Q_INT64_C(1000);

SimpleCryptDevice::SimpleCryptDevice(QSharedPointer<QIODevice> device, QObject *parent)
    : QIODevice(parent),
      m_crypt(SimpleCrypt::EMPTY_KEY),
      m_device(device)
{
    connect(device.data(), SIGNAL(aboutToClose()), this, SIGNAL(aboutToClose()));
    connect(device.data(), SIGNAL(bytesWritten(qint64)), this, SIGNAL(bytesWritten(qint64)));
    connect(device.data(), SIGNAL(readChannelFinished()), this, SIGNAL(readChannelFinished()));
    connect(device.data(), SIGNAL(readyRead()), this, SIGNAL(readyRead()));
}

SimpleCryptDevice::SimpleCryptDevice(QSharedPointer<QIODevice> device,
                                     quint64 key, QObject *parent)
    : QIODevice(parent),
      m_crypt(key),
      m_device(device)
{
    connect(device.data(), SIGNAL(aboutToClose()), this, SIGNAL(aboutToClose()));
    connect(device.data(), SIGNAL(bytesWritten(qint64)), this, SIGNAL(bytesWritten(qint64)));
    connect(device.data(), SIGNAL(readChannelFinished()), this, SIGNAL(readChannelFinished()));
    connect(device.data(), SIGNAL(readyRead()), this, SIGNAL(readyRead()));
}

SimpleCryptDevice::~SimpleCryptDevice() 
{ 
    
}

void SimpleCryptDevice::flush()
{
    if (isOpen() && !m_buffer.isEmpty()) {
        m_crypt.encodeSegment(m_device, m_buffer);
    }
}

qint64 SimpleCryptDevice::bytesAvailable() const
{
    return static_cast<qint64>(m_buffer.size()) +
            QIODevice::bytesAvailable() + m_device->bytesAvailable();
}

qint64 SimpleCryptDevice::bytesToWrite() const
{
    return static_cast<qint64>(m_buffer.size()) +
            QIODevice::bytesToWrite() + m_device->bytesToWrite();
}

void SimpleCryptDevice::close()
{
    flush();
    m_device->close();
    setOpenMode(QIODevice::NotOpen);
}

bool SimpleCryptDevice::open(OpenMode mode)
{
    bool modeInSync;
    if (m_device->isOpen()) {
        modeInSync = (m_device->openMode() != mode);
    } else {
        modeInSync = m_device->open(mode);
    }

    if (modeInSync) {
        setOpenMode(mode);
        return true;
    }
    return false;
}

qint64 SimpleCryptDevice::readData(char *data, qint64 maxSize)
{
    // early out
    if (maxSize == 0) {
        return 0;
    }
    // define data pointers
    char *it = data;
    char *end = (data + maxSize);
    // read buffer to output
    it += readBuffer(it, (end - it));
    // read segments to output
    while (!m_device->atEnd() && (it < end)) {
        if (m_crypt.decodeSegment(m_device, m_buffer) != SimpleCrypt::StreamOK) {
            break;
        }
        it += readBuffer(it, (end - it));
    }
    // return read byte count
    return (it - data);
}

qint64 SimpleCryptDevice::writeData(const char *data, qint64 maxSize)
{
    // early out
    if (maxSize == 0) {
        return 0;
    }
    // define data pointers
    const char *it = data;
    const char *end = (data + maxSize);
    // write input to segments
    while ((it < end)) {
        it += writeBuffer(it, qMin(static_cast<qint64>(end - it), DEFAULT_BUFFER_SIZE));
        if ((m_buffer.size() < DEFAULT_BUFFER_SIZE)
                || m_crypt.encodeSegment(m_device, m_buffer) != SimpleCrypt::StreamOK) {
            break;
        }
        m_buffer.clear();
    }
    // return written byte count
    return (it - data);
}

qint64 SimpleCryptDevice::readBuffer(char *out, qint64 maxSize)
{
    // early out
    if (maxSize == 0) {
        return 0;
    }
    QBuffer inDevice(&m_buffer);
    inDevice.open(QIODevice::ReadOnly);
    // read buffer to output, retain unread in buffer
    qint64 size = inDevice.read(out, maxSize);
    m_buffer = inDevice.readAll();
    inDevice.close();
    return size;
}

qint64 SimpleCryptDevice::writeBuffer(const char *in, qint64 maxSize)
{
    // early out
    if (maxSize == 0) {
        return 0;
    }
    QBuffer outDevice(&m_buffer);
    outDevice.open(QIODevice::WriteOnly);
    // read input to buffer, biting off a preset sized chunk
    qint64 size = outDevice.write(in, maxSize);
    outDevice.close();
    return size;
}

bool SimpleCryptDevice::seek(qint64 pos)
{
    Q_UNUSED(pos);
    //TODO implement
    return false;
}

qint64 SimpleCryptDevice::size() const
{
    return m_device->size();
}

qint64 SimpleCryptDevice::pos() const
{
    return m_device->pos();
}

bool SimpleCryptDevice::isSequential() const
{
    //TODO finish this
    return true;
}
