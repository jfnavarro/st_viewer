/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef USERDTO_H
#define USERDTO_H

#include <QObject>
#include <QString>

#include "dataModel/User.h"

// DTOs (Data transform objects) provides the bridge between the client
// specific data model objects and the server data. This is accomplished by
// defining dynamic properties that enable automated serialization and
// deserialization of server data.

class UserDTO : public QObject
{

public:

    Q_OBJECT

    Q_PROPERTY(QString username WRITE username)
    Q_PROPERTY(QString institution WRITE institution)
    Q_PROPERTY(QString first_name WRITE firstName)
    Q_PROPERTY(QString last_name WRITE lastName)
    Q_PROPERTY(QString street_address WRITE streetAddress)
    Q_PROPERTY(int postcode WRITE postcode)
    Q_PROPERTY(QString city WRITE city)
    Q_PROPERTY(QString country WRITE country)
    Q_PROPERTY(QString role WRITE role)
    Q_PROPERTY(QString password WRITE password)
    Q_PROPERTY(bool enabled WRITE enabled)

public:

    explicit UserDTO(QObject* parent = 0) : QObject(parent) { }
    ~UserDTO() { }

    // binding
    void username(const QString& username) { m_user.username(username); }
    void institution(const QString& institution) { m_user.institution(institution); }
    void firstName(const QString& firstName) { m_user.firstName(firstName); }
    void lastName(const QString& lastName) { m_user.secondName(lastName); }
    void streetAddress(const QString& streetAddress) { m_user.address(streetAddress); }
    void postcode(int postcode) { m_user.postcode(postcode); }
    void city(const QString& city) { m_user.city(city); }
    void country(const QString& country) { m_user.country(country); }
    void role(const QString& role) { m_user.role(role);}
    void password(const QString& password) { m_user.password(password); }
    void enabled(bool enabled) { m_user.enabled(enabled); }

    // get parsed data model
    const User& user() const { return m_user; }
    User& user() { return m_user; }

private:

    User m_user;
};

#endif // USERDTO_H //
