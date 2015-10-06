/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GENEEXPORTER_H
#define GENEEXPORTER_H

#include <QObject>
#include <QStringList>

#include "data/DataProxy.h"
#include "dataModel/UserSelection.h"

class QIODevice;
class QTextStream;

// Interface defining gene export (to text files) functionality
class GeneExporter
{
    Q_FLAGS(DetailLevel)
    Q_FLAGS(SeparationMode)

public:
    // exporter state enums
    enum DetailLevel {
        Simple = 0x00,
        Extended = 0x01,
        Comments = 0x10,
        // composite flags
        SimpleFull = Simple | Comments,
        ExtendedFull = Extended | Comments
    };
    Q_DECLARE_FLAGS(DetailLevels, DetailLevel)

    // separation mode for items in the file
    enum SeparationMode { TabDelimited, CommaDelimited };
    Q_DECLARE_FLAGS(SeparationModes, SeparationMode)

    GeneExporter();
    GeneExporter(DetailLevels detailLevel, SeparationModes separationMode);
    ~GeneExporter();

    // writes to the file given as input the selection items given as input
    void exportItem(QIODevice& device, const UserSelection::selectedGenesList& geneList) const;

    // to configure writing properties
    void addExportProperty(const QString& property);
    void addExportProperty(const QStringList& properties);

    // encode concatenated properties lists
    static const QString encodePropertyList(const QStringList& properties);
    static const QStringList decodePropertyList(const QString& properties);

protected:
    const QStringList exportPropertyList() const;
    const QString delimiterCharacter() const;

    // internal functions to process the individual selections
    void exportStrings(QTextStream& otxt, const QStringList& strings) const;
    void exportItem(QTextStream& otxt, const AggregatedGene& gene) const;
    void exportItem(QTextStream& otxt, const UserSelection::selectedGenesList& geneList) const;

    DetailLevels m_detailLevel;
    SeparationModes m_separationMode;
    QStringList m_propertyList;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GeneExporter::DetailLevels)
Q_DECLARE_OPERATORS_FOR_FLAGS(GeneExporter::SeparationModes)

#endif // GENEEXPORTER_H //
