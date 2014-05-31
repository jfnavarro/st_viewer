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
    ~User();

    User& operator=(const User& other);
    bool operator==(const User& other) const;

    const QString id() const;
    const QString username() const;
    const QString institution() const;
    const QString firstName() const;
    const QString secondName() const;
    const QString address() const;
    int postcode() const;
    const QString city() const;
    const QString country() const;
    const QString password() const;
    const QString role() const;
    bool enabled() const;

    void id(const QString& id);
    void username(const QString& username);
    void institution(const QString& institution);
    void firstName(const QString& firstName);
    void secondName(const QString& secondName);
    void address(const QString& address);
    void postcode(int postCode);
    void city(const QString& city);
    void country(const QString& country);
    void password(const QString& password);
    void role(const QString& role);
    void enabled(bool enabled);

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
