/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QMetaObject>
#include <QMetaProperty>
#include <QString>
#include <QLatin1String>
#include <QVariant>

#include <QDebug>

#include "dataModel/ObjectParser.h"

void ObjectParser::parseObject(const QVariant& source, QObject* target)
{
    if (!source.isValid() || !source.canConvert(QVariant::Map)) {
        qDebug() << "[ObjectParser] Error: Invalid object... " << source.toByteArray();
        return;
    }
    const QVariantMap& map = source.toMap();
    const QMetaObject *metaobject = target->metaObject();
    QVariantMap::const_iterator it, end = map.constEnd();
    for (it = map.constBegin(); it != end; ++it) {
        const int index = metaobject->indexOfProperty(it.key().toLatin1());
        if (index < 0) {
            continue;
        }
        const QMetaProperty metaproperty = metaobject->property(index);
        const QVariant::Type type = metaproperty.type();
        QVariant v = it.value();
        if (v.canConvert(type)) {
            v.convert(type);
            metaproperty.write(target, v);
        } else if (QString(QLatin1String("QVariant")).compare(QLatin1String(metaproperty.typeName())) == 0) {
            metaproperty.write(target, v);
        } else {
            qDebug() << "[ObjectParser] Warning: unable to map variable" << it.key() << "of type"
                     << v.type() << "to type" << metaproperty.type() << "!";
            metaproperty.write(target, QVariant());
        }
    }
}
