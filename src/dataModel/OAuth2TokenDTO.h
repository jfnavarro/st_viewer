/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef OAUTH2TOKENDTO_H
#define OAUTH2TOKENDTO_H

#include <QObject>

// DTOs (Data transform objects) provides the bridge between the client
// specific data model objects and the server data. This is accomplished by
// defining dynamic properties that enable automated serialization and
// deserialization of server data.

// this DTO parses the OAuth2 authorization token

//TODO move declarations to CPP and/or consider removing DTOs
class OAuth2TokenDTO : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString access_token READ accessToken WRITE accessToken)
    Q_PROPERTY(QString token_type READ tokenType WRITE tokenType)
    Q_PROPERTY(QString refresh_token READ refreshToken WRITE refreshToken)
    Q_PROPERTY(QString m_scope READ scope WRITE scope)
    Q_PROPERTY(int expires_in READ expiresIn WRITE expiresIn)

public:

    explicit OAuth2TokenDTO(QObject* parent = 0)
        : QObject(parent), m_accessToken(), m_refreshToken(), m_scopeType(), m_expiresIn(0) { }
    OAuth2TokenDTO(const OAuth2TokenDTO& other)
        : m_accessToken(other.accessToken()),
          m_refreshToken(other.refreshToken()),
          m_scopeType(other.scope()),
          m_expiresIn(other.expiresIn()) { }

    OAuth2TokenDTO& operator= (const OAuth2TokenDTO& other)
    {
        m_accessToken = other.accessToken();
        m_refreshToken = other.refreshToken();
        m_scopeType = other.scope();
        m_expiresIn = other.expiresIn();
        return (*this);
    }

    ~OAuth2TokenDTO() { }

    // binding
    const QString& accessToken() const { return m_accessToken; }
    const QString& tokenType() const { return m_tokenType; }
    const QString& refreshToken() const { return m_refreshToken; }
    const QString& scope() const { return m_scopeType; }
    int expiresIn() const { return m_expiresIn; }

    // getters
    void accessToken(const QString& accessToken) { m_accessToken = accessToken; }
    void tokenType(const QString& tokenType) { m_tokenType = tokenType; }
    void refreshToken(const QString& refreshToken) { m_refreshToken = refreshToken; }
    void scope(const QString& scope) { m_scopeType = scope; }
    void expiresIn(int expiresIn) { m_expiresIn = expiresIn; }

private:

    QString m_accessToken;
    QString m_tokenType;
    QString m_refreshToken;
    QString m_scopeType;
    int m_expiresIn;
};

#endif // OAUTH2TOKENDTO_H //
