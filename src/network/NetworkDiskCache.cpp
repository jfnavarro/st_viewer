/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "NetworkDiskCache.h"

#include <QDebug>
#include <QNetworkCacheMetaData>
#include <QDateTime>

NetworkDiskCache::NetworkDiskCache(QObject *parent) : QNetworkDiskCache(parent)
{
}

NetworkDiskCache::~NetworkDiskCache()
{

}


qint64 NetworkDiskCache::cacheSize() const
{
    return QNetworkDiskCache::cacheSize();
}

QIODevice* NetworkDiskCache::data(const QUrl &url)
{
    return QNetworkDiskCache::data(url);
}

void NetworkDiskCache::insert(QIODevice* device)
{
    return QNetworkDiskCache::insert(device);
}

QNetworkCacheMetaData NetworkDiskCache::metaData(const QUrl &url)
{
    QNetworkCacheMetaData metaData = QNetworkDiskCache::metaData(url);
    qDebug() << "Meta data for url " << url << " is " << metaData.rawHeaders();
    return metaData;
}

QIODevice* NetworkDiskCache::prepare(const QNetworkCacheMetaData &metaData)
{
    qDebug() << "Prepared data for meta data " << metaData.rawHeaders();

    QString mime;
    foreach (QNetworkCacheMetaData::RawHeader header, metaData.rawHeaders()) {
        if (header.first == "Content-Type") {
            mime = header.second;
            break;
        }
    }

    if (mime.startsWith("application/xml")
            || mime.startsWith("application/json")
            || mime.startsWith("image/jpeg")) {
        return QNetworkDiskCache::prepare(metaData);
    }

    return nullptr;
}

bool NetworkDiskCache::remove(const QUrl & url)
{
    const bool removed = QNetworkDiskCache::remove(url);
    qDebug() << "Removing data for url " << url << " yields " << removed;
    return removed;
}

void NetworkDiskCache::updateMetaData(const QNetworkCacheMetaData &metaData)
{
    qDebug() << "Updating meta data " << metaData.rawHeaders();
    return QNetworkDiskCache::updateMetaData(metaData);
}
