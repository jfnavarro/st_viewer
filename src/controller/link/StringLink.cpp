/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>
*/

#include "StringLink.h"

StringLink::StringLink(QObject* parent)
    : QObject(parent)
{

}

StringLink::~StringLink()
{

}

void StringLink::slotString(const QString& string)
{
    emit signalIsEmpty(string.isEmpty());

    //TODO add more as needed
}
