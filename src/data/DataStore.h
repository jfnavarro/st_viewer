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
#include <memory>

// The data store extend the resource store interface and provides simple
// functionality to manage temporarily stored files. Files stored through the
// data store are indexed and labelled so as to provide easy lookup. This makes
// it possible to check for previously stored temporary files despite the
// unique name prefix QT prepends.

//TODO the last modified info MAP is a temporary solution.
//The ideal solution is to have have a class that allows
//to store serialized (encrypted) files along with its last_modified info
//as well as objects. Use of QCache is adviced.
class DataStore : public QObject
{
    Q_OBJECT
    Q_FLAGS(Options)

public:

    typedef std::unique_ptr<QIODevice> resourceDeviceType;

    enum Option {
        Empty = 0x00,       // No options.
        Temporary = 0x01,   // Mark file as temporary.
        Persistent = 0x02,  // Make temporary file persistent.
        Secure = 0x04       // Encrypt cached data.
    };
    Q_DECLARE_FLAGS(Options, Option)

    explicit DataStore(QObject* parent = 0);
    ~DataStore();

    //check if the resource is stored already
    bool hasResource(const QString& resourceid) const;

    //check if the resource's last modified has changed
    //false = it has not changed or the last modified was not registered
    //true = it has changed
    //last modified info will be stored if it was not available
    bool resourceIsModified(const QString& resourceid,
                            qlonglong newLastModified);

    //access resource (returns nullptr if not present)
    resourceDeviceType accessResource(const QString& resourceid,
                                      Options options = Empty);

    //create resource with the given options and store the the last modified
    //info if given (returns the created resource)
    DataStore::resourceDeviceType createResource(const QString& resourceid,
                        DataStore::Options options = Empty,
                        qlonglong lastModified = -1);

    //remove all the stored resources (including last modified info)
    void clearResources();

private:

    //save and load resources and last modified into/from QSettings
    void loadResourceMap();
    void saveResourceMap();
    void loadLastModifiedMap();
    void saveLastModifiedMap();

    //create/open file resources and store them if necessary
    resourceDeviceType createFile(const QString& name, Options options);
    resourceDeviceType accessFile(const QString& name, Options options);

    //temporary files are prefixed with a set of characters so as to
    //guarantee its uniqueness.
    QMap<QString, QString> m_fileMap;

    //We want to store the last_modified information of each file in
    //a separate map
    QMap<QString, qlonglong> m_lastModifiedMap;

    Q_DISABLE_COPY(DataStore)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DataStore::Options)

#endif // DATASTORE_H //
