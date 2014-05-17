/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GENEEXPORTER_H
#define GENEEXPORTER_H

#include <QObject>
#include <QStringList>

#include "dataModel/GeneSelection.h"

class QIODevice;

// Interface defining gene export functionality
class GeneExporter : public QObject
{
public:

    explicit GeneExporter(QObject *parent = 0);
    virtual ~GeneExporter();

    virtual void exportItem(QIODevice *device, const GeneSelection& selectionList) const = 0;

    void addExportProperty(const QString& property);
    void addExportProperty(const QStringList& properties);

    static const QString encodePropertyList(const QStringList& properties);
    static const QStringList decodePropertyList(const QString& properties);

protected:

    const QStringList& exportPropertyList() const;

    QStringList m_propertyList;
};

#endif // GENEEXPORTER_H //
