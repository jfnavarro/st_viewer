#include "GeneExporter.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QTextStream>

static const QString PROPERTY_LIST_DELIMITER = QStringLiteral(";;");

GeneExporter::GeneExporter()
{
}

GeneExporter::GeneExporter(DetailLevels detailLevel, SeparationModes separationMode)
    : m_detailLevel(detailLevel)
    , m_separationMode(separationMode)
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

const QStringList GeneExporter::decodePropertyList(const QString& properties)
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

void GeneExporter::exportStrings(QTextStream& otxt, const QStringList& strings) const
{
    const QString delimiter = delimiterCharacter();
    otxt << strings.join(delimiter) << endl;
}

void GeneExporter::exportItem(QTextStream& otxt, const AggregatedGene& gene) const
{
    const qreal reads = gene.reads;
    const unsigned count = gene.count;
    const QString name = gene.name;

    QStringList list;
    list << QString("%1").arg(name) << QString("%1").arg(count) << QString("%1").arg(reads);

    exportStrings(otxt, list);
}

void GeneExporter::exportItem(QTextStream& otxt,
                              const UserSelection::selectedGenesList& geneList) const
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

    foreach (const AggregatedGene& gene, geneList) {
        exportItem(otxt, gene);
    }
}

void GeneExporter::exportItem(QIODevice& device,
                              const UserSelection::selectedGenesList& geneList) const
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

    exportItem(otxt, geneList);
}
