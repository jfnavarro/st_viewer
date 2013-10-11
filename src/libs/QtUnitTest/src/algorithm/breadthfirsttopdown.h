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
    // 
    // Example: The tree
    //           A
    //          / \
    //         B   C
    //        / \   \
    //       D   E   F
    //          / \
    //         G   H
    // yields the linearized list
    //   [A, B, C, D, E, F, G, H]
    // 
    // Note: QObject does not keep track of insertion order for children.
    class BreadthFirstTopDown : public Linearizer
    {
    public:
        virtual const QList<QObject *> list(QObject *object);
        BreadthFirstTopDown(){};
        virtual ~BreadthFirstTopDown(){};
    };

} // namespace unit //

#endif // BREADTHFIRSTTOPDOWN_H
