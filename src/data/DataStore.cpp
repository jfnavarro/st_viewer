/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "DataStore.h"

#include <QDebug>
#include <QTemporaryFile>
#include <QDir>
#include <QString>
#include <QSettings>

#include "data/SimpleCryptDevice.h"

static const QString TEMP_PREFIX = QStringLiteral("stvi_temp_XXXXXX_");
static const QString RESTORE_FILE = QStringLiteral("stvi_filemap");
static const quint64 ENCRYPT_KEY = 0xDEADC0DEBAADC0DE;

DataStore::DataStore(QObject *parent) :
    QObject(parent)
{
    loadResourceMap();
}

DataStore::~DataStore()
{
    saveResourceMap();
}

bool DataStore::hasResource(const QString& resourceid) const
{
    const bool ok = m_fileMap.contains(resourceid);
    qDebug() << QString("DataStore::hasResource(%1) = %2 ").arg(resourceid).arg(ok);
    return ok;
}

DataStore::resourceDeviceType
DataStore::accessResource(const QString& name, Options options)
{
    Q_ASSERT(!name.isNull() && !name.isEmpty());
    qDebug() << QString("DataStore::accessResource(%1, %2)").arg(name).arg(options);

    // load or create file with given resourceid and options
    DataStore::resourceDeviceType device;
    if (m_fileMap.contains(name)) {  
        device = std::move(accessFile(name, options));
    } else { 
        device = std::move(createFile(name, options));
    }
    Q_ASSERT(device.get());

    // add encryption layer if specified
    if (options.testFlag(Secure)) {
        SimpleCryptDevice *simpleCryptDevice =
                new SimpleCryptDevice(std::move(device), ENCRYPT_KEY);
        Q_ASSERT(simpleCryptDevice);
        device.reset(simpleCryptDevice);
    }

    return device;
}

void DataStore::clearResources()
{
    const QString restoreFile = QDir::temp().filePath(RESTORE_FILE);
    QSettings restore(restoreFile, QSettings::IniFormat);

    foreach(const QString & resourceid, restore.allKeys()) {
        const QString possibleFile = qvariant_cast<QString>(restore.value(resourceid, QString()));
        if (QFile::exists(possibleFile)) {
            QFile::remove(possibleFile);
            qDebug() << QString("[DataStore] Clear: (%1)").arg(possibleFile);
        } else {
            qDebug() << QString("[DataStore] Warning: Failed to clear file: %1").arg(possibleFile);
        }

        restore.remove(resourceid); //remove it from qsettigns as well
    }

    m_fileMap.clear();
}

void DataStore::loadResourceMap()
{
    const QString restoreFile = QDir::temp().filePath(RESTORE_FILE);
    QSettings restore(restoreFile, QSettings::IniFormat);

    foreach(const QString & resourceid, restore.allKeys()) {
        const QString possibleFile = qvariant_cast<QString>(restore.value(resourceid, QString()));
        if (QFile::exists(possibleFile)) {
            m_fileMap[resourceid] = qvariant_cast<QString>(restore.value(resourceid, QString()));
            qDebug() << QString("[DataStore] Load: (%1 -> %2)").arg(resourceid).arg(m_fileMap[resourceid]);
        } else {
            qDebug() << QString("[DataStore] Warning: Failed to load file: %1").arg(possibleFile);
        }
    }

}

void DataStore::saveResourceMap()
{
    const QString restoreFile = QDir::temp().filePath(RESTORE_FILE);
    QSettings restore(restoreFile, QSettings::IniFormat);

    foreach(const QString & resourceid, m_fileMap.keys()) {
        qDebug() << QString("[DataStore] Save: (%1 -> %2)").arg(resourceid).arg(m_fileMap[resourceid]);
        restore.setValue(resourceid, m_fileMap[resourceid]);
    }
}

DataStore::resourceDeviceType DataStore::createFile(const QString& name, Options options)
{
    Q_ASSERT(! name.isNull() && ! name.isEmpty());
    qDebug() << QString("DataStore::createFile(%1, %2)").arg(name).arg(options);

    // early out
    if (m_fileMap.contains(name)) {
        return accessFile(name, options);
    }

    std::unique_ptr<QFile> file;
    if (options.testFlag(Option::Temporary)) {
        const QString filePath = QDir::temp().filePath(TEMP_PREFIX + name);
        std::unique_ptr<QTemporaryFile> tempFile(new QTemporaryFile(filePath));
        // apply options
        tempFile->setAutoRemove(!options.testFlag(Option::Persistent));
        // force file name generation
        tempFile->open();
        tempFile->close();
        file.reset(tempFile.release());
    } else {
        const QString filePath = QDir::current().filePath(name);
        file.reset(new QFile(filePath));
    }

    // save filename map
    const QString filename = file->fileName();
    qDebug() << QString("[DataStore] Map: (%1 -> %2)").arg(name).arg(filename);
    m_fileMap[name] = filename;

    return DataStore::resourceDeviceType(file.release());
}

DataStore::resourceDeviceType DataStore::accessFile(const QString& name, Options options)
{
    qDebug() << QString("DataStore::accessFile(%1, %2)").arg(name).arg(options);

    // early out
    FileMap::const_iterator it = m_fileMap.find(name);
    if (it == m_fileMap.end()) {
        return nullptr;
    }

    return resourceDeviceType(new QFile(it.value(), this));
}
