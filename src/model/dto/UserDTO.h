/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef USERDTO_H
#define USERDTO_H

#include <QObject>
#include <QString>

#include "model/User.h"

// DTOs (Data transform objects) provides the bridge between the client
// specific data model objects and the server data. This is accomplished by
// defining dynamic properties that enable automated serialization and
// deserialization of server data.

// User defines the parsing object for the underlying User data object.
// Mapping Notes:
//     1:1 mapping. No conversions.
class UserDTO : public QObject
{

public:

    Q_OBJECT

    Q_PROPERTY(QString username READ username WRITE username)
    Q_PROPERTY(QString role READ role WRITE role)

public:

    explicit UserDTO(QObject* parent = 0);
    UserDTO(const User& user, QObject* parent = 0);
    virtual ~UserDTO();

    // binding
    inline const QString& username() const { return m_user.username(); }
    inline const QString& role() const { return m_user.role(); }

    inline void username(const QString& username) { m_user.username(username); }
    inline void role(const QString& role) { m_user.role(role); }

    // get parsed data model
    const User& user() const { return m_user; }
    User& user() { return m_user; }

private:

    User m_user;
};

#endif // USERDTO_H //
