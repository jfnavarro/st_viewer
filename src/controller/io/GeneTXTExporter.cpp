/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GeneTXTExporter.h"

#include <QIODevice>

#include <QCoreApplication>
#include <QDateTime>

GeneTXTExporter::GeneTXTExporter(QObject *parent)
    : GeneExporter(parent),
      m_detailLevel(Simple),
      m_separationMode(TabDelimited)
{

}

GeneTXTExporter::GeneTXTExporter(DetailLevels detailLevel,
                                 SeparationModes separationMode, QObject *parent)
    : GeneExporter(parent),
      m_detailLevel(detailLevel),
      m_separationMode(separationMode)
{

}

GeneTXTExporter::~GeneTXTExporter()
{

}

const QString GeneTXTExporter::delimiterCharacter() const
{
    switch (m_separationMode) {
    case TabDelimited:
        return QStringLiteral("\t");
    case CommaDelimited:
    default:
        return QStringLiteral(",");
    }
}

void GeneTXTExporter::exportStrings(QTextStream &otxt, const QStringList &strings) const
{
    const QString delimiter = delimiterCharacter();
    otxt << strings.join(delimiter) << endl;
}

void GeneTXTExporter::exportItem(QTextStream &otxt, const DataProxy::FeaturePtr &feature,
                                 const QObject &context) const
{
    QStringList list;
    const int hitCount = feature->hits();
    list << QString("%1").arg(feature->gene())
         << QString("%1").arg(hitCount);
    const bool hasNormalized =
        context.property("hitCountMin").isValid()
        && context.property("hitCountMax").isValid();
    if (hasNormalized) {
        const int hitCountMin = qvariant_cast<int>(context.property("hitCountMin"));
        const int hitCountMax = qvariant_cast<int>(context.property("hitCountMax"));
        list << QString("%1").arg(qreal(hitCount - hitCountMin) / qreal(hitCountMax - hitCountMin));
    }
    exportStrings(otxt, list);
}

void GeneTXTExporter::exportItem(QTextStream &otxt, const DataProxy::FeatureListPtr featureList,
                                 const QObject &context) const
{
    const bool hasNormalized =
        context.property("hitCountMin").isValid()
        && context.property("hitCountMax").isValid();
    // prepend header
    if (m_detailLevel.testFlag(GeneTXTExporter::Comments)) {
        QStringList list;
        list << tr("gene_name")
             << tr("reads_count");
        if (hasNormalized) {
            list << tr("normalized_reads_count");
        }
        otxt << QString("# ");
        exportStrings(otxt, list);
    }
    foreach(const DataProxy::FeaturePtr & feature, (*featureList)) {
        exportItem(otxt, feature, context);
    }
}

void GeneTXTExporter::exportItem(QIODevice *device, const DataProxy::FeatureListPtr featureList,
                                 const QObject &context) const
{
    // early out
    if (!device->isWritable()) {
        return;
    }
    QTextStream otxt(device);
    // prepend header
    if (m_detailLevel.testFlag(GeneTXTExporter::Extended)) {
        // identifying comment
        if (m_detailLevel.testFlag(GeneTXTExporter::Comments)) {
            otxt << QString("# %1").arg(tr("stVi export: feature list")) << endl;
        }

        // prepend data with application version
        const QString version = QCoreApplication::applicationVersion();
        if (m_detailLevel.testFlag(GeneTXTExporter::Comments)) {
            otxt << QString("# %1").arg(tr("version")) << endl;
        }
        otxt << (version.isEmpty() ? QString("0.0.0") : version) << endl;

        // prepend ISO 8601 compliant timestamp
        if (m_detailLevel.testFlag(GeneTXTExporter::Comments)) {
            otxt << QString("# %1 (UTC)").arg(tr("date")) << endl;
        }
        otxt << QDateTime::currentDateTimeUtc().toString(Qt::ISODate) << endl;
    }
    exportItem(otxt, featureList, context);
}
