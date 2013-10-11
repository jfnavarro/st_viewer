/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "Error.h"

Error::Error(QObject* parent)
    : QObject(parent), m_name(), m_description()
{

}

Error::Error(const QString& name, const QString& description, QObject* parent)
    : QObject(parent), m_name(name), m_description(description)
{

}

Error::~Error()
{

}
