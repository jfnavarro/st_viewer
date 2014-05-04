/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GeneXMLExporter.h"

#include <QIODevice>
#include <QCoreApplication>
#include <QDateTime>
#include <QVariant>

#include "dataModel/GeneSelection.h"

GeneXMLExporter::GeneXMLExporter(QObject *parent) : GeneExporter(parent)
{

}

GeneXMLExporter::~GeneXMLExporter()
{

}

void GeneXMLExporter::exportItem(QXmlStreamWriter &oxml,
                                 const GeneSelection &selection) const
{
    oxml.writeStartElement("feature");
    {
        oxml.writeTextElement("geneName", selection.name());
        const qreal reads = selection.reads();
        oxml.writeTextElement("readsCount", QString("%1").arg(reads));
        const qreal normalizedReads = selection.normalizedReads();
        oxml.writeTextElement("normalizedReadsCount", QString("%1").arg(normalizedReads));
    }
    oxml.writeEndElement();
}
void GeneXMLExporter::exportItem(QXmlStreamWriter &oxml,
                                 const DataProxy::UniqueGeneSelectedList &selectionList) const
{
    oxml.writeStartElement("features");
    foreach(const GeneSelection& selection, selectionList) {
        exportItem(oxml, selection);
    }
    oxml.writeEndElement();
}

void GeneXMLExporter::exportItem(QIODevice *device,
                                 const DataProxy::UniqueGeneSelectedList &selectionList) const
{
    // early out
    if (!device->isWritable()) {
        return;
    }
    QXmlStreamWriter oxml(device);
    oxml.setAutoFormatting(true);
    oxml.setAutoFormattingIndent(4);
    oxml.writeStartDocument();
    {
        oxml.writeStartElement("root");
        {
            // identifying comment
            oxml.writeComment(tr("stVi export: feature list"));
            // prepend data with application version
            const QString version = QCoreApplication::applicationVersion();
            oxml.writeTextElement(tr("version"), (version.isEmpty() ? QString("0.0.0") : version));
            // prepend ISO 8601 compliant timestamp
            oxml.writeTextElement("date", QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
            exportItem(oxml, selectionList);
        }
        oxml.writeEndElement();
    }
    oxml.writeEndDocument();
}
