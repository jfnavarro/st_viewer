/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "User.h"

User::User()
    : m_username(""), m_role("")
{

}

User::User(const User& other)
{
    m_username = other.m_username;
    m_role = other.m_role;
}

User::User(const QString& name, const QString& role)
    : m_username(name), m_role(role)
{

}

User::~User()
{

}

User& User::operator=(const User& other)
{
    m_username = other.m_username;
    m_role = other.m_role;

    return (*this);
}

bool User::operator==(const User& other) const
{
    if (
        m_username == other.m_username &&
        m_role == other.m_role
    )
        return true;
    else
        return false;
}
