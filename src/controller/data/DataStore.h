/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef DATASTORE_H
#define DATASTORE_H

#include <QObject>
#include <QMap>
#include <QFile>

#include "utils/Singleton.h"

#include "ResourceStore.h"

class Error;

// The data store extend the resource store interface and provides simple
// functionality to manage temporarily stored files. Files stored through the
// data store are indexed and labelled so as to provide easy lookup. This makes
// it possible to check for previously stored temporary files despite the
// unique name prefix QT prepends.
class DataStore : public ResourceStore, public Singleton<DataStore>
{
    Q_OBJECT

public:

    explicit DataStore(QObject* parent = 0);
    virtual ~DataStore();

    void init();
    void finalize();

    // store resources on disk, provide interface to store in files
    virtual bool hasResource(const QString& resourceid) const;
    virtual QIODevice* accessResource(const QString& resourceid, Options options = Empty);
    virtual void clearResources();

signals:

    void signalError(Error* error);

private:

    void loadResourceMap();
    void saveResourceMap();

    QIODevice* createFile(const QString& name, Options options);
    QIODevice* accessFile(const QString& name, Options options);
    //NOTE temporary files are prefixed with a set of characters so as to
    // guarantee its uniqueness. This map provides a means of mapping the
    // requested file name with the actual name.
    // It is made persistent by storing and loading it along with the
    // temporary files.
    typedef QMap<QString, QString> FileMap;
    FileMap m_fileMap;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DataStore::Options)

#endif // DATASTORE_H //
