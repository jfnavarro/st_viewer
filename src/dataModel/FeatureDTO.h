#ifndef FEATUREDTO_H
#define FEATUREDTO_H

#include <QObject>
#include <QString>

#include "dataModel/Feature.h"

// DTOs (Data transform objects) provides the bridge between the client
// specific data model objects and the server data. This is accomplished by
// defining dynamic properties that enable automated serialization and
// deserialization of server data.

// TODO move definitions to CPP and/or consider removing DTOs
class FeatureDTO : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString gene READ gene WRITE gene)
    Q_PROPERTY(int hits READ count WRITE count)
    Q_PROPERTY(int x READ x WRITE x)
    Q_PROPERTY(int y READ y WRITE y)

public:
    explicit FeatureDTO(QObject *parent = 0)
        : QObject(parent)
    {
    }
    ~FeatureDTO() {}

    // binding
    void gene(const QString &gene) { m_feature.gene(gene); }
    void count(int count) { m_feature.count(count); }
    void x(int x) { m_feature.x(x); }
    void y(int y) { m_feature.y(y); }

    // read
    const QString gene() { return m_feature.gene(); }
    int count() { return m_feature.count(); }
    int x() { return m_feature.x(); }
    int y() { return m_feature.y(); }

    // get parsed data model
    const Feature &feature() const { return m_feature; }
    Feature &feature() { return m_feature; }

private:
    Feature m_feature;
};

#endif // FEATUREDTO_H //
