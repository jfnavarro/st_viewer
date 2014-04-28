/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GENETXTEXPORTER_H
#define GENETXTEXPORTER_H

#include <QTextStream>
#include <QString>

#include "GeneExporter.h"

// Concrete class implementing the gene exporter interface. Exports genes as
// plain text. Allows for some customization by adding detail levels and option
// to choose delimiter character.
class GeneTXTExporter : public GeneExporter
{

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

    enum SeparationMode {
        TabDelimited,
        CommaDelimited
    };
    Q_DECLARE_FLAGS(SeparationModes, SeparationMode)

    explicit GeneTXTExporter(QObject *parent = 0);
    explicit GeneTXTExporter(DetailLevels detailLevel,
                             SeparationModes separationMode, QObject *parent = 0);
    virtual ~GeneTXTExporter();

    virtual void exportItem(QIODevice *device,
                            const GeneRendererGL::GeneSelectedSet) const;

private:

    const QString delimiterCharacter() const;
    void exportStrings(QTextStream &otxt, const QStringList &strings) const;
    void exportItem(QTextStream &otxt, const GeneSelection &selection) const;
    void exportItem(QTextStream &otxt, const GeneRendererGL::GeneSelectedSet& selectionList) const;

    DetailLevels m_detailLevel;
    SeparationModes m_separationMode;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GeneTXTExporter::DetailLevels)
Q_DECLARE_OPERATORS_FOR_FLAGS(GeneTXTExporter::SeparationModes)

#endif // GENETXTEXPORTER_H //
