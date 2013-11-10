/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "UserExperimentDTO.h"

UserExperimentDTO::UserExperimentDTO(QObject* parent) : QObject(parent), m_userExperiment()
{
    
}

UserExperimentDTO::UserExperimentDTO(const UserExperiment& userExperiment, QObject* parent) :
    QObject(parent), m_userExperiment(userExperiment)
{
    
}

UserExperimentDTO::~UserExperimentDTO()
{
    
}
