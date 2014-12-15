/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GeneExporter.h"

#include <QCoreApplication>
#include <QDateTime>

static const QString PROPERTY_LIST_DELIMITER = QStringLiteral(";;");

GeneExporter::GeneExporter()
{

}

GeneExporter::GeneExporter(DetailLevels detailLevel, SeparationModes separationMode)
    :  m_detailLevel(detailLevel),
      m_separationMode(separationMode)
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

const QString GeneExporter::delimiterCharacter() const
{
    switch (m_separationMode) {
    case TabDelimited:
        return QStringLiteral("\t");
    case CommaDelimited:
    default:
        return QStringLiteral(",");
    }
}

void GeneExporter::exportStrings(QTextStream &otxt, const QStringList &strings) const
{
    const QString delimiter = delimiterCharacter();
    otxt << strings.join(delimiter) << endl;
}

void GeneExporter::exportItem(QTextStream &otxt,
                              const SelectionType& selection) const
{
    const qreal reads = selection.reads;
    const int count = selection.count;
    const QString name = selection.name;

    QStringList list;
    list << QString("%1").arg(name)
         << QString("%1").arg(count)
         << QString("%1").arg(reads);

    exportStrings(otxt, list);
}

void GeneExporter::exportItem(QTextStream &otxt,
                              const GeneSelection::selectedItemsList& selectionList) const
{
    // prepend header
    if (m_detailLevel.testFlag(GeneExporter::Comments)) {
        QStringList list;
        list << "gene_name"
             << "gene_count"
             << "reads_count";
        otxt << QString("# ");
        exportStrings(otxt, list);
    }

    foreach(const SelectionType &selection, selectionList) {
        exportItem(otxt, selection);
    }
}

void GeneExporter::exportItem(QIODevice &device,
                              const GeneSelection::selectedItemsList& selectionList) const
{
    // early out
    if (!device.isWritable()) {
        return;
    }

    QTextStream otxt(&device);

    // prepend header
    if (m_detailLevel.testFlag(GeneExporter::Extended)) {
        // identifying comment
        if (m_detailLevel.testFlag(GeneExporter::Comments)) {
            otxt << QString("# %1").arg("ST Viewer export: feature list") << endl;
        }

        // prepend data with application version
        const QString version = QCoreApplication::applicationVersion();
        if (m_detailLevel.testFlag(GeneExporter::Comments)) {
            otxt << QString("# %1").arg("version") << endl;
        }

        otxt << (version.isEmpty() ? QString("0.0.0") : version) << endl;

        // prepend ISO 8601 compliant timestamp
        if (m_detailLevel.testFlag(GeneExporter::Comments)) {
            otxt << QString("# %1 (UTC)").arg("date") << endl;
        }

        otxt << QDateTime::currentDateTimeUtc().toString(Qt::ISODate) << endl;
    }

    exportItem(otxt, selectionList);
}
