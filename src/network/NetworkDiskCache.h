/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef NETWORKDISKCACHE_H
#define NETWORKDISKCACHE_H

#include <QNetworkDiskCache>

class NetworkDiskCache : public QNetworkDiskCache
{

public:

    explicit NetworkDiskCache(QObject *parent = 0);
    virtual ~NetworkDiskCache();

    virtual qint64 cacheSize() const;
    virtual QIODevice* data(const QUrl &url);
    virtual void insert(QIODevice *device);
    virtual QNetworkCacheMetaData metaData(const QUrl &url);
    virtual QIODevice* prepare(const QNetworkCacheMetaData &metaData);
    virtual bool remove(const QUrl &url);
    virtual void updateMetaData(const QNetworkCacheMetaData &metaData);
};

#endif // NETWORKDISKCACHE_H
