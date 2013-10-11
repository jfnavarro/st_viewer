/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef SOURCEGENERATOR_H
#define SOURCEGENERATOR_H

#include <QByteArray>

namespace unit
{

    class SourceGenerator
    {
    public:
        virtual QByteArray generate(qint64 maxSize) const = 0;
    };

} // namespace unit //

#endif // SOURCEGENERATOR_H //
