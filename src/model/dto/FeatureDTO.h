/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef FEATUREDTO_H
#define FEATUREDTO_H

#include <QObject>
#include <QString>

#include "model/Feature.h"

// DTOs (Data transform objects) provides the bridge between the client
// specific data model objects and the server data. This is accomplished by
// defining dynamic properties that enable automated serialization and
// deserialization of server data.

// FeatureDTO defines the parsing object for the underlying Feature data
// object.
// Mapping Notes:
//     1:1 mapping. No conversions.
class FeatureDTO : public QObject
{
  
public:
  
    Q_OBJECT

    Q_PROPERTY(QString id READ id WRITE id)
    Q_PROPERTY(QString barcode READ barcode WRITE barcode)
    Q_PROPERTY(QString gene READ gene WRITE gene)
    Q_PROPERTY(int hits READ hits WRITE hits)
    Q_PROPERTY(int x READ x WRITE x)
    Q_PROPERTY(int y READ y WRITE y)

private:
  
    Q_DISABLE_COPY(FeatureDTO)

public:
  
    explicit FeatureDTO(QObject* parent = 0);
    FeatureDTO(const Feature& feature, QObject* parent = 0);
    virtual ~FeatureDTO();

    // binding
    inline const QString& id() const { return m_feature.id(); }
    inline const QString& barcode() const { return m_feature.barcode(); }
    inline const QString& gene() const { return m_feature.gene(); }
    inline int hits() const { return m_feature.hits(); }
    inline int x() const { return m_feature.x(); }
    inline int y() const { return m_feature.y(); }

    inline void id(const QString& id) { m_feature.id(id); }
    inline void barcode(const QString& barcode) { m_feature.barcode(barcode); }
    inline void gene(const QString& gene) { m_feature.gene(gene); }
    inline void hits(int hits) { m_feature.hits(hits); }
    inline void x(int x) { m_feature.x(x); }
    inline void y(int y) { m_feature.y(y); }

    // get parsed data model
    const Feature& feature() const { return m_feature; }
    Feature& feature() { return m_feature; }

private:
  
    Feature m_feature;
};

#endif // FEATUREDTO_H //
