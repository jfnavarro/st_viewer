/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "OAuth2TokenDTO.h"

OAuth2TokenDTO::OAuth2TokenDTO(QObject* parent)
    : QObject(parent), m_accessToken(), m_refreshToken(), m_expiresIn(0)
{

}

OAuth2TokenDTO::~OAuth2TokenDTO()
{

}
