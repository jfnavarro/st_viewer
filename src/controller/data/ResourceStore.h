/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef RESOURCESTORE_H
#define RESOURCESTORE_H

#include <QObject>
#include <QString>
#include <QIODevice>

// ResourceStore provides an interface to store and access resources
class ResourceStore : public QObject
{
    Q_FLAGS(Options)

public:

    enum Option {
        Empty = 0x00,       // No options.
        Temporary = 0x01,   // Mark file as temporary.
        Persistent = 0x02,  // Make temporary file persistent.
        Secure = 0x04       // Encrypt cached data.
    };
    Q_DECLARE_FLAGS(Options, Option)

    ResourceStore(QObject *parent = 0) : QObject(parent) { }

    virtual bool hasResource(const QString& resourceid) const = 0;
    virtual QIODevice* accessResource(const QString& resourceid, Options options = Empty) = 0;
    virtual void clearResources() = 0;
};

#endif // RESOURCESTORE_H //
