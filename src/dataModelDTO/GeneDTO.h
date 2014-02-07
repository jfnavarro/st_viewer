/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GENEDTO_H
#define GENEDTO_H

#include <QObject>
#include <QString>

#include "dataModel/Gene.h"

// DTOs (Data transform objects) provides the bridge between the client
// specific data model objects and the server data. This is accomplished by
// defining dynamic properties that enable automated serialization and
// deserialization of server data.

// GeneDTO defines the parsing object for the underlying Gene data object.
// Mapping Notes:
//     1:1 mapping. No conversions.
class GeneDTO : public QObject
{

public:

    Q_OBJECT

    Q_PROPERTY(QString id READ id WRITE id)
    Q_PROPERTY(QString gene READ name WRITE name) //NOTE gene name is called "gene" on server side

public:

    explicit GeneDTO(QObject* parent = 0);
    explicit GeneDTO(const Gene& gene, QObject* parent = 0);
    virtual ~GeneDTO();

    // binding
    const QString& id() const { return m_gene.id(); }
    const QString& name() const { return m_gene.name(); }

    void id(const QString& id) { m_gene.id(id); }
    void name(const QString& name) { m_gene.name(name); }

    // get parsed data model
    const Gene& gene() const { return m_gene; }
    Gene& gene() { return m_gene; }

private:

    Gene m_gene;
};

#endif // GENEDTO_H //
