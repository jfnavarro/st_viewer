/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef ASCIIGENERATOR_H
#define ASCIIGENERATOR_H

#include "sourcegenerator.h"

namespace unit
{

class AsciiGenerator : public SourceGenerator
{
public:
    virtual QByteArray generate(qint64 maxSize) const;
};

} // namespace unit //

#endif // ASCIIGENERATOR_H //
