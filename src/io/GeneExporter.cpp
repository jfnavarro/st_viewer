/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GeneExporter.h"

static const QString PROPERTY_LIST_DELIMITER = QStringLiteral(";;");

GeneExporter::GeneExporter(QObject *parent) : QObject(parent)
{

}

GeneExporter::~GeneExporter()
{

}

void GeneExporter::addExportProperty(const QString& property)
{
    m_propertyList.append(property);
}

void GeneExporter::addExportProperty(const QStringList& properties)
{
    m_propertyList.append(properties);
}

const QString GeneExporter::encodePropertyList(const QStringList& properties)
{
    return properties.join(PROPERTY_LIST_DELIMITER);
}

const QStringList GeneExporter::decodePropertyList(const QString &properties)
{
    return properties.split(PROPERTY_LIST_DELIMITER, QString::SkipEmptyParts);
}
