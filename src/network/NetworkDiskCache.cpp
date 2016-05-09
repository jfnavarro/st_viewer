#include "NetworkDiskCache.h"

#include <QDebug>
#include <QNetworkCacheMetaData>
#include <QDateTime>

NetworkDiskCache::NetworkDiskCache(QObject *parent)
    : QNetworkDiskCache(parent)
{
}

NetworkDiskCache::~NetworkDiskCache()
{
}

qint64 NetworkDiskCache::cacheSize() const
{
    return QNetworkDiskCache::cacheSize();
}

QIODevice *NetworkDiskCache::data(const QUrl &url)
{
    return QNetworkDiskCache::data(url);
}

void NetworkDiskCache::insert(QIODevice *device)
{
    return QNetworkDiskCache::insert(device);
}

QNetworkCacheMetaData NetworkDiskCache::metaData(const QUrl &url)
{
    return QNetworkDiskCache::metaData(url);
}

QIODevice *NetworkDiskCache::prepare(const QNetworkCacheMetaData &metaData)
{
    QString mime;
    for (QNetworkCacheMetaData::RawHeader header : metaData.rawHeaders()) {
        if (header.first == "Content-Type") {
            mime = header.second;
            break;
        }
    }

    // only cache jpeg/xml and json content
    // TODO this will have to be updated when we add binnary format
    if (mime.startsWith("application/xml") || mime.startsWith("application/json")
        || mime.startsWith("image/jpeg") || mime.startsWith("application/x-gzip")) {
        return QNetworkDiskCache::prepare(metaData);
    }

    return nullptr;
}

bool NetworkDiskCache::remove(const QUrl &url)
{
    return QNetworkDiskCache::remove(url);
}

void NetworkDiskCache::updateMetaData(const QNetworkCacheMetaData &metaData)
{
    return QNetworkDiskCache::updateMetaData(metaData);
}
