/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QDebug>
#include "utils/DebugHelper.h"

#include "DatasetDTO.h"

DatasetDTO::DatasetDTO(QObject* parent) : QObject(parent), m_dataset()
{
    
}

DatasetDTO::DatasetDTO(const Dataset& dataset, QObject* parent) : QObject(parent), m_dataset(dataset)
{
    
}

DatasetDTO::~DatasetDTO()
{
    
}

const QVariantList DatasetDTO::serializeTransform(const QTransform& transform) const
{
    QVariantList serializedTransform;
    // serialize data
    serializedTransform
            << QVariant::fromValue(transform.m11())
            << QVariant::fromValue(transform.m12())
            << QVariant::fromValue(transform.m13())
            << QVariant::fromValue(transform.m21())
            << QVariant::fromValue(transform.m22())
            << QVariant::fromValue(transform.m23())
            << QVariant::fromValue(transform.m31())
            << QVariant::fromValue(transform.m32())
            << QVariant::fromValue(transform.m33());
    return serializedTransform;
}

const QTransform DatasetDTO::unserializeTransform(const QVariantList& serializedTransform) const
{
    QTransform transform(Qt::Uninitialized);

    // unserialize data
    QVector<qreal> values;
    QVariantList::const_iterator it, end = serializedTransform.end();
    for (it = serializedTransform.begin(); it != end; ++it)
    {
        values << it->value<qreal>();
    }

    // parse transform matrix
    QVector<qreal>::size_type size = values.size();
    switch (size)
    {
    // 3x2 afinite transform matrix
    case 6:
        transform = QTransform(
                    values[0], values[3],
                values[1], values[4],
                values[2], values[5]
                );
        break;
        // normal 3x3 transform matrix
    case 9:
        transform = QTransform(
                    values[0], values[3], values[6],
                values[1], values[4], values[7],
                values[2], values[5], values[8]
                );
        break;
        // meh...
    default:
        qDebug() << "[DatasetDTO] Warning: Unable to unserialize transform matrix!" << endl << serializedTransform;
        transform = QTransform();
        break;
    }

    return transform;
}
