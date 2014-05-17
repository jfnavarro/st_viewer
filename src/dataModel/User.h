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
    virtual ~User();

    User& operator=(const User& other);
    bool operator==(const User& other) const;

    const QString id() const { return m_id; }
    const QString username() const { return m_username; }
    const QString institution() const { return m_instituion; }
    const QString firstName() const { return m_firstName; }
    const QString secondName() const { return m_secondName; }
    const QString address() const { return m_address; }
    int postcode() const { return m_postcode; }
    const QString city() const { return m_city; }
    const QString country() const { return m_country; }
    const QString password() const { return m_password; }
    const QString role() const { return m_role; }
    bool enabled() const { return m_enabled; }

    void id(const QString& id) { m_id = id; }
    void username(const QString& username) { m_username = username; }
    void institution(const QString& institution) { m_instituion = institution; }
    void firstName(const QString& firstName) { m_firstName = firstName; }
    void secondName(const QString& secondName) { m_secondName = secondName; }
    void address(const QString& address) { m_address = address; }
    void postcode(int postCode) { m_postcode = postCode; }
    void city(const QString& city) { m_city = city; }
    void country(const QString& country) { m_country = country; }
    void password(const QString& password) { m_password = password; }
    void role(const QString& role) { m_role = role; }
    void enabled(bool enabled) { m_enabled = enabled; }

private:

    QString m_id;
    QString m_username;
    QString m_instituion;
    QString m_firstName;
    QString m_secondName;
    QString m_address;
    int m_postcode;
    QString m_city;
    QString m_country;
    QString m_password;
    QString m_role;
    bool m_enabled;
};

#endif // USER_H
