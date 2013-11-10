/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/


#include <QDebug>
#include "utils/DebugHelper.h"

#include "utils/Utils.h"

#include <QTemporaryFile>
#include <QDir>
#include <QString>
#include <QSettings>

#include <core/data/SimpleCryptDevice.h>

#include "DataStore.h"

DataStore::DataStore(QObject* parent)
    : ResourceStore(parent)
{
    
}

DataStore::~DataStore()
{
    
}

void DataStore::init()
{
    loadResourceMap();
}

void DataStore::finalize()
{
    saveResourceMap();
}

bool DataStore::hasResource(const QString& resourceid)
{
    bool ok = m_fileMap.contains(resourceid);
    qDebug() << QString("DataStore::hasResource(%1) = %2 ").arg(resourceid).arg(ok);
    return ok;
}

QIODevice* DataStore::accessResource(const QString& name, Options options)
{
    qDebug() << QString("DataStore::accessResource(%1, %2)").arg(name).arg(options);

    // load or create file with given resourceid and options
    QIODevice *device = (m_fileMap.contains(name)) ?
                accessFile(name, options) :
                createFile(name, options);

    // add encryption layer if specified
    if (options.testFlag(Secure))
    {
        device = new SimpleCryptDevice(device, 0xDEADC0DEBAADC0DE, this);
    }

    return device;
}

void DataStore::clearResources()
{
    DEBUG_FUNC_NAME

    const QString restoreFile = QDir::temp().filePath(Globals::RESTORE_FILE);
    QSettings restore(restoreFile, QSettings::IniFormat);
    
    foreach (const QString& resourceid, restore.allKeys())
    {
        const QString possibleFile = qvariant_cast<QString>(restore.value(resourceid, QString()));
        if (QFile::exists(possibleFile))
        {
            QFile::remove(possibleFile);
            qDebug() << QString("[DataStore] Clear: (%1)").arg(possibleFile);
        }
        else
        {
            qDebug() << QString("[DataStore] Warning: Failed to clear file: %1").arg(possibleFile);
        }
        restore.remove(resourceid); //remove it from qsettigns as well
    }
    
    m_fileMap.clear();
}

void DataStore::loadResourceMap()
{
    const QString restoreFile = QDir::temp().filePath(Globals::RESTORE_FILE);
    QSettings restore(restoreFile, QSettings::IniFormat);
    
    foreach (const QString& resourceid, restore.allKeys())
    {
        const QString possibleFile = qvariant_cast<QString>(restore.value(resourceid, QString()));
        if (QFile::exists(possibleFile))
        {
            m_fileMap[resourceid] = qvariant_cast<QString>(restore.value(resourceid, QString()));
            qDebug() << QString("[DataStore] Load: (%1 -> %2)").arg(resourceid).arg(m_fileMap[resourceid]);
        }
        else
        {
            qDebug() << QString("[DataStore] Warning: Failed to load file: %1").arg(possibleFile);
        }
    }
}
void DataStore::saveResourceMap()
{
    const QString restoreFile = QDir::temp().filePath(Globals::RESTORE_FILE);
    QSettings restore(restoreFile, QSettings::IniFormat);
    
    foreach (const QString& resourceid, m_fileMap.keys())
    {
        qDebug() << QString("[DataStore] Save: (%1 -> %2)").arg(resourceid).arg(m_fileMap[resourceid]);
        restore.setValue(resourceid, m_fileMap[resourceid]);
    }
}

QIODevice* DataStore::createFile(const QString& name, Options options)
{
    qDebug() << QString("DataStore::createFile(%1, %2)").arg(name).arg(options);

    // early out
    if (m_fileMap.contains(name))
    {
        return accessFile(name, options);
    }

    QFile* file;
    if (options.testFlag(ResourceStore::Temporary))
    {
        const QString filePath = QDir::temp().filePath(Globals::TEMP_PREFIX + name);
        QTemporaryFile* tempFile = new QTemporaryFile(filePath, this);

        // apply options
        tempFile->setAutoRemove(!options.testFlag(ResourceStore::Persistent));

        // force file name generation
        tempFile->open();
        tempFile->close();
        
        file = tempFile;
    }
    else
    {
        const QString filePath = QDir::current().filePath(name);
        file = new QFile(filePath, this);
    }

    // save filename map
    QString filename = file->fileName();
    qDebug() << QString("[DataStore] Map: (%1 -> %2)").arg(name).arg(filename);
    m_fileMap[name] = filename;

    return file;
}

QIODevice* DataStore::accessFile(const QString& name, Options options)
{
    qDebug() << QString("DataStore::accessFile(%1, %2)").arg(name).arg(options);

    // early out
    FileMap::const_iterator it = m_fileMap.find(name);
    if (it == m_fileMap.end())
    {
        return 0;
    }

    QString filename = it.value();
    return new QFile(filename, this);
}
