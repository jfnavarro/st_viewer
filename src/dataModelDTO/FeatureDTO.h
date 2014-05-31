/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef FEATUREDTO_H
#define FEATUREDTO_H

#include <QObject>
#include <QString>

#include "dataModel/Feature.h"

// DTOs (Data transform objects) provides the bridge between the client
// specific data model objects and the server data. This is accomplished by
// defining dynamic properties that enable automated serialization and
// deserialization of server data.

class FeatureDTO : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString id WRITE id)
    Q_PROPERTY(QString barcode WRITE barcode)
    Q_PROPERTY(QString gene WRITE gene)
    Q_PROPERTY(QString annotation WRITE annotation)
    Q_PROPERTY(int hits WRITE hits)
    Q_PROPERTY(int x WRITE x)
    Q_PROPERTY(int y WRITE y)

public:

    explicit FeatureDTO(QObject* parent = 0) : QObject(parent) {}
    ~FeatureDTO() {}

    // binding
    void id(const QString& id) {  m_feature.id(id); }
    void barcode(const QString& barcode) { m_feature.barcode(barcode); }
    void gene(const QString& gene) { m_feature.gene(gene); }
    void annotation(const QString& annotation) { m_feature.annotation(annotation); }
    void hits(int hits) { m_feature.hits(hits); }
    void x(int x) { m_feature.x(x); }
    void y(int y) { m_feature.y(y); }

    // get parsed data model
    const Feature& feature() const { return m_feature; }
    Feature& feature() { return m_feature; }

private:

    Feature m_feature;
};

#endif // FEATUREDTO_H //
