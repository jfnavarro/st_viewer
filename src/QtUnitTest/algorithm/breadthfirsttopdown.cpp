/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QLinkedList>

#include "breadthfirsttopdown.h"

namespace unit
{

BreadthFirstTopDown::BreadthFirstTopDown() 
{
    
}

BreadthFirstTopDown::~BreadthFirstTopDown() 
{
    
}
    
const QList<QObject *> BreadthFirstTopDown::list(QObject *object)
{
    QList<QObject *> list;

    // add root layer
    if (object) {
        list.append(object);
    }

    // go through list and append children to the back
    for (int i = 0; i < list.size(); ++i) {
        // append current layer
        list.append(list[i]->children());
    }
    return list;
}

} // namespace unit //
