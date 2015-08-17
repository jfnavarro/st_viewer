/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef SERIALIZATIONFUNCTIONS_H
#define SERIALIZATIONFUNCTIONS_H

#include <QVector>
#include <QVariantList>

// Utility templated functions to convert from QVariantList of any type
// to QVector and viceversa
//TODO maybe separate definition and declaration
template<typename N>
const QVariantList serializeVector(const QVector<N>& unserializedVector)
{
    QVariantList newList;
    foreach(const N &item, unserializedVector.toList()) {
        newList << QVariant::fromValue(item);
    }
    return newList;
}

template<typename N>
const QVector<N> unserializeVector(const QVariantList &serializedVector)
{
    // unserialize data
    QVector<N> values;
    QVariantList::const_iterator it;
    QVariantList::const_iterator end = serializedVector.end();
    for (it = serializedVector.begin(); it != end; ++it) {
        values.push_back(it->value<N>());
    }
    return values;
}

#endif // SERIALIZATIONFUNCTIONS_H
