/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef BINARYGENERATOR_H
#define BINARYGENERATOR_H

#include "sourcegenerator.h"

namespace unit
{

class BinaryGenerator : public SourceGenerator
{
public:
    virtual QByteArray generate(qint64 maxSize) const;
};

} // namespace unit //

#endif // BINARYGENERATOR_H //
