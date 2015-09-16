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

// TODO move definitions to CPP and/or consider removing DTOs
class FeatureDTO : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString barcode READ barcode WRITE barcode)
    Q_PROPERTY(QString gene READ gene WRITE gene)
    Q_PROPERTY(QString annotation READ annotation WRITE annotation)
    Q_PROPERTY(int hits READ hits WRITE hits)
    Q_PROPERTY(double x READ x WRITE x)
    Q_PROPERTY(double y READ y WRITE y)

public:
    explicit FeatureDTO(QObject* parent = 0)
        : QObject(parent)
    {
    }
    ~FeatureDTO() {}

    // binding
    void barcode(const QString& barcode) { m_feature.barcode(barcode); }
    void gene(const QString& gene) { m_feature.gene(gene); }
    void annotation(const QString& annotation) { m_feature.annotation(annotation); }
    void hits(int hits) { m_feature.hits(hits); }
    void x(double x) { m_feature.x(x); }
    void y(double y) { m_feature.y(y); }

    // read
    const QString barcode() { return m_feature.barcode(); }
    const QString gene() { return m_feature.gene(); }
    const QString annotation() { return m_feature.annotation(); }
    int hits() { return m_feature.hits(); }
    double x() { return m_feature.x(); }
    double y() { return m_feature.y(); }

    // get parsed data model
    const Feature& feature() const { return m_feature; }
    Feature& feature() { return m_feature; }

private:
    Feature m_feature;
};

#endif // FEATUREDTO_H //
