/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef LINEARIZER_H
#define LINEARIZER_H

#include <QObject>
#include <QList>

namespace unit
{

    // Linearizer is an interface defining algorithms to linearize the parent
    // child tree structure defined by QObject.
    class Linearizer
    {
    public:
        virtual const QList<QObject *> list(QObject *object) = 0;
        Linearizer(){};
        virtual ~Linearizer(){};
    };
    
} // namespace unit //

#endif // LINEARIZER_H
