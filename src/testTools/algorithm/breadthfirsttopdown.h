/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef BREADTHFIRSTTOPDOWN_H
#define BREADTHFIRSTTOPDOWN_H

#include "linearizer.h"

namespace unit
{

// BreadthFirstTopDown implements a breadth-first, top-down linearization
// algorithm that operate on the parent-child tree structure defined by
// QObject.
// Note: QObject does not keep track of insertion order for children.
class BreadthFirstTopDown : public Linearizer
{

public:
    BreadthFirstTopDown();

    virtual ~BreadthFirstTopDown();

    virtual const QList<QObject*> list(QObject* object);
};

} // namespace unit //

#endif // BREADTHFIRSTTOPDOWN_H
