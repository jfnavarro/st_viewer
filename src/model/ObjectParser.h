/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef OBJECTPARSER_H
#define OBJECTPARSER_H

#include <QObject>
#include <QVariantMap>

#include <QDebug>

// The object parser is used to initialize or construct QObjects using their
// meta data properties as targets and a variant data structure as the source.
// The variant source, which can be either a QVariant or a QVariantMap, is
// mapped to the properties of the QObject trying to convert the variant data
// to the data type of the mapped object member variable.
// Currently the parsing only supports flat structures.
class ObjectParser
{

public:

    // parse an object from variant map using intermediary DTO type
    static void parseObject(const QVariant& source, QObject* target);
};

#endif // OBJECTPARSER_H //
