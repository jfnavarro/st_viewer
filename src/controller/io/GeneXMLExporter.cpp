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

GeneXMLExporter::GeneXMLExporter(QObject *parent) : GeneExporter(parent)
{

}

GeneXMLExporter::~GeneXMLExporter()
{

}

void GeneXMLExporter::exportItem(QXmlStreamWriter &oxml, const DataProxy::FeaturePtr &feature,
                                 const QObject &context) const
{
    oxml.writeStartElement("feature");
    {
        oxml.writeTextElement("geneName", feature->gene());
        const int hitCount = feature->hits();
        oxml.writeTextElement("readsCount", QString("%1").arg(hitCount));

        const bool hasNormalized = context.property("hitCountMax").isValid();
        if (hasNormalized) {
            const int hitCountMax = qvariant_cast<int>(context.property("hitCountMax"));
            oxml.writeTextElement("normalizedReadsCount", QString("%1").arg(qreal(hitCount) / qreal(hitCountMax)));
        }
    }
    oxml.writeEndElement();
}
void GeneXMLExporter::exportItem(QXmlStreamWriter &oxml, const DataProxy::FeatureListPtr featureList,
                                 const QObject &context) const
{
    oxml.writeStartElement("features");
    foreach(const DataProxy::FeaturePtr & feature, (*featureList)) {
        exportItem(oxml, feature, context);
    }
    oxml.writeEndElement();
}

void GeneXMLExporter::exportItem(QIODevice *device, const DataProxy::FeatureListPtr featureList,
                                 const QObject &context) const
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
            exportItem(oxml, featureList, context);
        }
        oxml.writeEndElement();
    }
    oxml.writeEndDocument();
}
