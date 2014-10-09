/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "NetworkCommand.h"

#include <QVariant>
#include <QMetaProperty>
#include <QDebug>
#include <QString>
#include <QJsonArray>

NetworkCommand::NetworkCommand(QObject* parent) : QObject(parent),
    m_url(),
    m_type(Globals::HttpRequestTypeNone),
    m_query(),
    m_body()
{

}

NetworkCommand::NetworkCommand(const QUrl& url,
                               Globals::HttpRequestType type,
                               QObject* parent) :
    QObject(parent),
    m_url(url),
    m_type(type),
    m_query(),
    m_body()
{

}

NetworkCommand::~NetworkCommand()
{

}

void NetworkCommand::addQueryItems(QObject *object)
{
    Q_ASSERT(object != nullptr);

    // extract the objects meta data
    const QMetaObject* metaObject = object->metaObject();
    const int size = metaObject->propertyCount();

    for (int i = metaObject->propertyOffset(); i < size; ++i) {
        const QMetaProperty metaproperty = metaObject->property(i);
        // ignore if not readable
        const QString  param = metaproperty.name();
        if (!metaproperty.isReadable()) {
            qDebug() << "[NetworkCommand] Warning: The property "
                     << metaproperty.typeName()
                     << param << "is not readable and will be ignored!";
            continue;
        }

        // ignore if not convertable
        const QVariant value = metaproperty.read(object);
        if (!value.canConvert(QVariant::String)) {
            qDebug() << "[NetworkCommand] Warning: The property "
                     << metaproperty.typeName() << param
                     << " is not convertable to QString, and will be ignored!";
            continue;
        }

        // convert and add property
        m_query.addQueryItem(param, value.toString());
    }
}

const QUrl& NetworkCommand::url() const
{
    return m_url;
}

Globals::HttpRequestType NetworkCommand::type() const
{
    return m_type;
}

const QUrlQuery& NetworkCommand::query() const
{
    return m_query;
}

const QString NetworkCommand::getEncodedUrl() const
{
    QUrl url(m_url);
    url.setQuery(m_query);
    return url.toString(QUrl::FullyEncoded);
}

void NetworkCommand::setBody(const QByteArray &body)
{
    m_body = body;
}

const QByteArray NetworkCommand::body() const
{
    return m_body;
}

void NetworkCommand::addQueryItem(const QString& param, const QString& value)
{
    m_query.addQueryItem(param, value);
}

void NetworkCommand::addQueryItem(const QString& param, const int value)
{
    m_query.addQueryItem(param, QString::number(value));
}

void NetworkCommand::addQueryItem(const QString& param, const qreal value)
{
    m_query.addQueryItem(param, QString::number(value));
}

const QString NetworkCommand::getQueryItem(const QString& param) const
{
    return m_query.queryItemValue(param);
}

const QString NetworkCommand::getEncodedQuery() const
{
    return m_query.query(QUrl::FullyEncoded);
}
