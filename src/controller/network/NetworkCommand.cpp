/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "NetworkCommand.h"
#include <QVariant>
#include <QMetaProperty>

#include <QDebug>

NetworkCommand::NetworkCommand(QObject* parent) : QObject(parent), 
    m_url(), m_type(Globals::HttpRequestTypeNone), m_query()
{ 
    
}
NetworkCommand::NetworkCommand(const QUrl& url, Globals::HttpRequestType type, QObject* parent) : 
    QObject(parent), m_url(url), m_type(type), m_query()
{ 
    
}
NetworkCommand::~NetworkCommand() 
{ 
    
}

void NetworkCommand::addQueryItems(QObject* object)
{
    Q_ASSERT(object != 0 && "null-pointer assertion error!");

    // extract the objects meta data
    const QMetaObject* metaObject = object->metaObject();

    int size = metaObject->propertyCount();
    for(int i = metaObject->propertyOffset(); i < size; ++i)
    {
        QMetaProperty metaproperty = metaObject->property(i);

        // abort if not readable
        QString  param = metaproperty.name();
        if (!metaproperty.isReadable())
        {
            qDebug() << "[NetworkCommand] Warning: The property" << metaproperty.typeName()
                     << param << "is not readable and will be ignored!";
            continue;
        }
        
        // abort if not convertable
        QVariant value = metaproperty.read(object);
        if (!value.canConvert(QVariant::String))
        {
            qDebug() << "[NetworkCommand] Warning: The property" << metaproperty.typeName() << param
                     << "is not convertable to QString, and will be ignored!";
            continue;
        }

        // convert and add property
        m_query.addQueryItem(param, value.toString());
    }
}

const QString NetworkCommand::getEncodedUrl() const
{
    QUrl url(m_url);
    url.setQuery(m_query);
    return url.toString(QUrl::FullyEncoded);
}


