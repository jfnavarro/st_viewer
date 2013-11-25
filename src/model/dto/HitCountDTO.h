/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef HITCOUNTDTO_H
#define HITCOUNTDTO_H

#include <QObject>

#include "model/HitCount.h"

// DTOs (Data transform objects) provides the bridge between the client
// specific data model objects and the server data. This is accomplished by
// defining dynamic properties that enable automated serialization and
// deserialization of server data.

// HitCount defines the parsing object for the underlying HitCount data object.
// Mapping Notes:
//     1:1 mapping. No conversions.
class HitCountDTO : public QObject
{

public:

    Q_OBJECT

    Q_PROPERTY(int min READ min WRITE min)
    Q_PROPERTY(int max READ max WRITE max)
    Q_PROPERTY(int sum READ sum WRITE sum)

public:

    explicit HitCountDTO(QObject* parent = 0);
    HitCountDTO(const HitCount& hitCount, QObject* parent = 0);
    virtual ~HitCountDTO();

    // binding
    inline int min() const
    {
        return m_hitCount.min();
    }
    inline int max() const
    {
        return m_hitCount.max();
    }
    inline int sum() const
    {
        return m_hitCount.sum();
    }

    inline void min(int min)
    {
        m_hitCount.min(min);
    }
    inline void max(int max)
    {
        m_hitCount.max(max);
    }
    inline void sum(int sum)
    {
        m_hitCount.sum(sum);
    }

    // get parsed data model
    const HitCount& hitCount() const
    {
        return m_hitCount;
    }
    HitCount& hitCount()
    {
        return m_hitCount;
    }

private:

    HitCount m_hitCount;
};

#endif // HITCOUNTDTO_H //
