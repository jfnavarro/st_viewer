/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "UserDTO.h"

UserDTO::UserDTO(QObject* parent) : QObject(parent), m_user()
{
    
}

UserDTO::UserDTO(const User& user, QObject* parent) : QObject(parent), m_user(user)
{
    
}

UserDTO::~UserDTO()
{
    
}
