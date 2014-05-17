/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "User.h"

User::User()
    : m_id(),
      m_username(),
      m_instituion(),
      m_firstName(),
      m_address(),
      m_postcode(0),
      m_city(),
      m_country(),
      m_password(),
      m_role(),
      m_enabled(false)
{

}


User::User(const User& other)
    : m_id(other.m_id),
      m_username(other.m_username),
      m_instituion(other.m_instituion),
      m_firstName(other.m_firstName),
      m_address(other.m_address),
      m_postcode(other.m_postcode),
      m_city(other.m_city),
      m_country(other.m_country),
      m_password(other.m_password),
      m_role(other.m_role),
      m_enabled(other.m_enabled)
{

}

User::~User()
{

}

User& User::operator=(const User& other)
{
    m_id = other.m_id;
    m_username = other.m_username;
    m_instituion = other.m_instituion;
    m_firstName = other.m_firstName;
    m_address = other.m_address;
    m_postcode = other.m_postcode;
    m_city = other.m_city;
    m_country = other.m_country;
    m_password = other.m_password;
    m_role = other.m_role;
    m_enabled = other.m_enabled;
    return (*this);
}

bool User::operator==(const User& other) const
{
    return (
                m_id == other.m_id &&
                m_username == other.m_username &&
                m_instituion == other.m_instituion &&
                m_firstName == other.m_firstName &&
                m_address == other.m_address &&
                m_postcode == other.m_postcode &&
                m_city == other.m_city &&
                m_country == other.m_country &&
                m_password == other.m_password &&
                m_role == other.m_role &&
                m_enabled == other.m_enabled
        );
}
