/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GENEXMLEXPORTER_H
#define GENEXMLEXPORTER_H

#include <QXmlStreamWriter>

#include "GeneExporter.h"

// Concrete class implementing the gene exporter interface. Exports genes in
// xml format.
class GeneXMLExporter : public GeneExporter
{
public:

    explicit GeneXMLExporter(QObject *parent = 0);
    virtual ~GeneXMLExporter();

    virtual void exportItem(QIODevice *device,
                            const GeneRendererGL::GeneSelectedSet selectionList) const;

private:

    void exportItem(QXmlStreamWriter &oxml,
                    const GeneSelection &selection) const;

    void exportItem(QXmlStreamWriter &oxml,
                    const GeneRendererGL::GeneSelectedSet selectionList) const;
};

#endif // GENEXMLEXPORTER_H //
