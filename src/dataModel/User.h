/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef USER_H
#define USER_H

#include <QString>

// Data model class to store user data.
class User
{
    
public:
    
    User();
    explicit User(const User& other);
    explicit User(const QString& name, const QString& role);
    virtual ~User();

    User& operator=(const User& other);
    bool operator==(const User& other) const;

    const QString& username() const { return m_username; }
    const QString& role() const { return m_role; }

    void username(const QString& username) { m_username = username; }
    void role(const QString& role) { m_role = role; }

private:

    QString m_username;
    QString m_role;
};

#endif // USER_H
