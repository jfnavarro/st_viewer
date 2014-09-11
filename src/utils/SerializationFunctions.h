#ifndef SERIALIZATIONFUNCTIONS_H
#define SERIALIZATIONFUNCTIONS_H

#include <QVector>
#include <QVariantList>

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
