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
static const QString LAST_MODIFIED_FILE = QStringLiteral("stvi_lastmodifiedmap");
static const quint64 ENCRYPT_KEY = 0xDEADC0DEBAADC0DE;

DataStore::DataStore(QObject *parent) :
    QObject(parent)
{
    loadResourceMap();
    loadLastModifiedMap();
}

DataStore::~DataStore()
{
    saveResourceMap();
    saveLastModifiedMap();
}

bool DataStore::hasResource(const QString& resourceid) const
{
    const bool ok = m_fileMap.contains(resourceid);
    qDebug() << QString("DataStore::hasResource(%1) = %2 ").arg(resourceid).arg(ok);
    return ok;
}

bool DataStore::resourceIsModified(const QString &resourceid, qlonglong newLastModified)
{
    if (!m_lastModifiedMap.contains(resourceid)) {
        qDebug() << QString("DataStore::resourceIsModified() Resource is not present %1").
                    arg(resourceid);
        return false;
    }

    const qlonglong storedLastModified = m_lastModifiedMap[resourceid];
    //-1 is the default value for the last modified which means
    //that it was not given when the file was created
    //thus, we add it now
    if (storedLastModified == -1) {
        m_lastModifiedMap[resourceid] = newLastModified;
        qDebug() << QString("DataStore::resourceIsModified() Adding last modified for %1 = %2").
                    arg(resourceid).arg(newLastModified);
        return false;
    }

    const bool ok = m_lastModifiedMap[resourceid] != newLastModified;
    qDebug() << QString("DataStore::resourceIsModified() for %1 = %2").arg(resourceid).arg(ok);
    return ok;
}

DataStore::resourceDeviceType
DataStore::accessResource(const QString& name, Options options)
{
    Q_ASSERT(!name.isNull() && !name.isEmpty());
    qDebug() << QString("DataStore::accessResource(%1, %2)").arg(name).arg(options);

    // load file with given resourceid and options
    DataStore::resourceDeviceType device = std::move(accessFile(name, options));

    // add encryption layer if specified and resource was present
    if (device.get() && options.testFlag(Secure)) {
        SimpleCryptDevice *simpleCryptDevice =
                new SimpleCryptDevice(std::move(device), ENCRYPT_KEY);
        Q_ASSERT(simpleCryptDevice);
        device.reset(simpleCryptDevice);
    }

    return device;
}

DataStore::resourceDeviceType
DataStore::createResource(const QString &name, Options options, qlonglong lastModified)
{
    Q_ASSERT(!name.isNull() && !name.isEmpty());
    qDebug() << QString("DataStore::createResource(%1, %2)").arg(name).arg(options);

    // create file with given resourceid and options
    DataStore::resourceDeviceType device = std::move(createFile(name, options));
    Q_ASSERT(device.get());

    // add encryption layer if specified
    if (options.testFlag(Secure)) {
        SimpleCryptDevice *simpleCryptDevice =
                new SimpleCryptDevice(std::move(device), ENCRYPT_KEY);
        Q_ASSERT(simpleCryptDevice);
        device.reset(simpleCryptDevice);
    }

    // update the last modified map
    m_lastModifiedMap[name] = lastModified;

    return device;
}

void DataStore::clearResources()
{
    const QString restoreFile = QDir::temp().filePath(RESTORE_FILE);
    QSettings restore(restoreFile, QSettings::IniFormat);
    //remove files
    foreach(const QString &resourceid, restore.allKeys()) {
        const QString possibleFile = qvariant_cast<QString>(restore.value(resourceid, QString()));
        if (QFile::exists(possibleFile)) {
            QFile::remove(possibleFile);
            qDebug() << QString("[DataStore] Clear: (%1)").arg(possibleFile);
        } else {
            qDebug() << QString("[DataStore] Warning: Failed to clear file: %1").arg(possibleFile);
        }
        //remove it from qsettigns as well
        restore.remove(resourceid);
    }
    //clear resource map and last modified map
    m_fileMap.clear();
    m_lastModifiedMap.clear();
}

void DataStore::loadResourceMap()
{
    const QString restoreFile = QDir::temp().filePath(RESTORE_FILE);
    QSettings restore(restoreFile, QSettings::IniFormat);
    //load files into resource map
    foreach(const QString &resourceid, restore.allKeys()) {
        const QString possibleFile = restore.value(resourceid, QString()).toString();
        if (QFile::exists(possibleFile)) {
            m_fileMap[resourceid] = possibleFile;
            qDebug() << QString("[DataStore] Load File: (%1 -> %2)").arg(resourceid).arg(possibleFile);
        } else {
            qDebug() << QString("[DataStore] Warning: Failed to load file: %1").arg(possibleFile);
        }
    }

}

void DataStore::saveResourceMap()
{
    const QString restoreFile = QDir::temp().filePath(RESTORE_FILE);
    QSettings restore(restoreFile, QSettings::IniFormat);
    //save content of the resource map
    foreach(const QString &resourceid, m_fileMap.keys()) {
        qDebug() << QString("[DataStore] Save File: (%1 -> %2)").
                    arg(resourceid).arg(m_fileMap[resourceid]);
        restore.setValue(resourceid, m_fileMap[resourceid]);
    }
}

void DataStore::loadLastModifiedMap()
{
    const QString lastModifiedFile = QDir::temp().filePath(LAST_MODIFIED_FILE);
    QSettings restore(lastModifiedFile, QSettings::IniFormat);
    //load files into resource map
    foreach(const QString &resourceid, restore.allKeys()) {
        const qlonglong storedLastModified = restore.value(resourceid, -1).toLongLong();
        m_lastModifiedMap[resourceid] = storedLastModified;
        qDebug() << QString("[DataStore] Load Last Modified: (%1 -> %2)").
                    arg(resourceid).arg(storedLastModified);
    }

}

void DataStore::saveLastModifiedMap()
{
    const QString lastModifiedFile = QDir::temp().filePath(LAST_MODIFIED_FILE);
    QSettings restore(lastModifiedFile, QSettings::IniFormat);
    //save content of the resource map
    foreach(const QString &resourceid, m_lastModifiedMap.keys()) {
        qDebug() << QString("[DataStore] Save Last Modified: (%1 -> %2)")
                    .arg(resourceid).arg(m_lastModifiedMap[resourceid]);
        restore.setValue(resourceid, m_lastModifiedMap[resourceid]);
    }
}

DataStore::resourceDeviceType DataStore::createFile(const QString& name, Options options)
{
    Q_ASSERT(!name.isNull() && !name.isEmpty());
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
    QMap<QString, QString>::iterator it = m_fileMap.find(name);
    return it == m_fileMap.end() ? nullptr : resourceDeviceType(new QFile(it.value(), this));
}
