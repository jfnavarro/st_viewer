#ifndef NETWORKDISKCACHE_H
#define NETWORKDISKCACHE_H

#include <QNetworkDiskCache>

// Wrapper around QNetworkDiskCache to control
// what type of content can be cached to avoid to always have
// to download content from the internet
class NetworkDiskCache : public QNetworkDiskCache
{

public:
    explicit NetworkDiskCache(QObject* parent = 0);
    virtual ~NetworkDiskCache();

    virtual qint64 cacheSize() const;
    virtual QIODevice* data(const QUrl& url);
    virtual void insert(QIODevice* device);
    virtual QNetworkCacheMetaData metaData(const QUrl& url);
    virtual QIODevice* prepare(const QNetworkCacheMetaData& metaData);
    virtual bool remove(const QUrl& url);
    virtual void updateMetaData(const QNetworkCacheMetaData& metaData);
};

#endif // NETWORKDISKCACHE_H
