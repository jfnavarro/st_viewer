/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "FeatureExporter.h"

#include <QCoreApplication>

#include "dataModel/Feature.h"

static const QString PROPERTY_LIST_DELIMITER = QStringLiteral(";;");

FeatureExporter::FeatureExporter()
{

}

FeatureExporter::FeatureExporter(DetailLevels detailLevel, SeparationModes separationMode)
    :  m_detailLevel(detailLevel),
      m_separationMode(separationMode)
{

}

FeatureExporter::~FeatureExporter()
{

}

void FeatureExporter::addExportProperty(const QString& property)
{
    m_propertyList.append(property);
}

void FeatureExporter::addExportProperty(const QStringList& properties)
{
    m_propertyList.append(properties);
}

const QString FeatureExporter::encodePropertyList(const QStringList& properties)
{
    return properties.join(PROPERTY_LIST_DELIMITER);
}

const QStringList FeatureExporter::decodePropertyList(const QString &properties)
{
    return properties.split(PROPERTY_LIST_DELIMITER, QString::SkipEmptyParts);
}

const QString FeatureExporter::delimiterCharacter() const
{
    switch (m_separationMode) {
    case TabDelimited:
        return QStringLiteral("\t");
    case CommaDelimited:
    default:
        return QStringLiteral(",");
    }
}

void FeatureExporter::exportStrings(QTextStream &otxt, const QStringList &strings) const
{
    const QString delimiter = delimiterCharacter();
    otxt << strings.join(delimiter) << endl;
}

void FeatureExporter::exportItem(QTextStream &otxt,
                              const Feature& feature) const
{
    QStringList list;
    list << QString("%1").arg(feature.gene())
         << QString("%1").arg(feature.barcode())
         << QString("%1").arg(feature.x())
         << QString("%1").arg(feature.y())
         << QString("%1").arg(feature.hits());

    exportStrings(otxt, list);
}

void FeatureExporter::exportItem(QTextStream &otxt,
                              const DataProxy::FeatureList& featureList) const
{
    // prepend header
    if (m_detailLevel.testFlag(FeatureExporter::Comments)) {
        QStringList list;
        list << "gene_name"
             << "barcode_id"
             << "x"
             << "y"
             << "reads_count";
        otxt << QString("# ");
        exportStrings(otxt, list);
    }

    foreach(const DataProxy::FeaturePtr feature, featureList) {
        exportItem(otxt, *feature);
    }
}

void FeatureExporter::exportItem(QIODevice &device,
                              const DataProxy::FeatureList& featureList) const
{
    // early out
    if (!device.isWritable()) {
        return;
    }

    QTextStream otxt(&device);

    // prepend header
    if (m_detailLevel.testFlag(FeatureExporter::Extended)) {
        // identifying comment
        if (m_detailLevel.testFlag(FeatureExporter::Comments)) {
            otxt << QString("# %1").arg("stVi export: feature list") << endl;
        }
        // prepend data with application version
        const QString version = QCoreApplication::applicationVersion();
        if (m_detailLevel.testFlag(FeatureExporter::Comments)) {
            otxt << QString("# %1").arg("version") << endl;
        }
        otxt << (version.isEmpty() ? QString("0.0.0") : version) << endl;
        // prepend ISO 8601 compliant timestamp
        if (m_detailLevel.testFlag(FeatureExporter::Comments)) {
            otxt << QString("# %1 (UTC)").arg("date") << endl;
        }
        otxt << QDateTime::currentDateTimeUtc().toString(Qt::ISODate) << endl;
    }

    exportItem(otxt, featureList);
}

