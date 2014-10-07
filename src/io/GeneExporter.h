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
#include "dataModel/GeneSelection.h"

class QIODevice;

// Interface defining gene export functionality
class GeneExporter
{
    Q_FLAGS(DetailLevel)
    Q_FLAGS(SeparationMode)

public:
    // exporter state enums
    enum DetailLevel {
        Simple   = 0x00,
        Extended = 0x01,
        Comments = 0x10,
        // composite flags
        SimpleFull = Simple | Comments,
        ExtendedFull = Extended | Comments
    };
    Q_DECLARE_FLAGS(DetailLevels, DetailLevel)

    //separation mode for items in the file
    enum SeparationMode {
        TabDelimited,
        CommaDelimited
    };
    Q_DECLARE_FLAGS(SeparationModes, SeparationMode)

    GeneExporter();
    GeneExporter(DetailLevels detailLevel, SeparationModes separationMode);
    ~GeneExporter();

    void exportItem(QIODevice &device,
                    const GeneSelection::selectedItemsList& selectionList) const;

    void addExportProperty(const QString& property);
    void addExportProperty(const QStringList& properties);

    static const QString encodePropertyList(const QStringList& properties);
    static const QStringList decodePropertyList(const QString& properties);

protected:

    const QStringList exportPropertyList() const;
    const QString delimiterCharacter() const;
    void exportStrings(QTextStream &otxt, const QStringList &strings) const;
    void exportItem(QTextStream &otxt, const SelectionType &selection) const;
    void exportItem(QTextStream &otxt,
                     const GeneSelection::selectedItemsList& selectionList) const;

    DetailLevels m_detailLevel;
    SeparationModes m_separationMode;
    QStringList m_propertyList;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GeneExporter::DetailLevels);
Q_DECLARE_OPERATORS_FOR_FLAGS(GeneExporter::SeparationModes);

#endif // GENEEXPORTER_H //
