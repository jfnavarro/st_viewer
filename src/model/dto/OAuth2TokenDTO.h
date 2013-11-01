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

// OAuth2Token defines the parsing object for the underlying OAuth2Token data
// object.
// Mapping Notes:
//     1:1 mapping. No conversions.
class OAuth2TokenDTO : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString access_token READ accessToken WRITE accessToken)
    Q_PROPERTY(QString token_type READ tokenType WRITE tokenType)
    Q_PROPERTY(QString refresh_token READ refreshToken WRITE refreshToken)
    Q_PROPERTY(QString m_scope READ scope WRITE scope)
    Q_PROPERTY(int expires_in READ expiresIn WRITE expiresIn)
    

public:
    explicit OAuth2TokenDTO(QObject* parent = 0);
    virtual ~OAuth2TokenDTO();

    // binding
    inline const QString& accessToken() const { return m_accessToken; }
    inline const QString& tokenType() const { return m_tokenType; }
    inline const QString& refreshToken() const { return m_refreshToken; }
    inline const QString& scope() const { return m_scope; }
    inline const int expiresIn() const { return m_expiresIn; }
    // getters
    inline void accessToken(const QString& accessToken) { m_accessToken = accessToken; }
    inline void tokenType(const QString& tokenType) { m_tokenType = tokenType; }
    inline void refreshToken(const QString& refreshToken) { m_refreshToken = refreshToken; }
    inline void scope(const QString& scope) { m_scope = scope; }
    inline void expiresIn(int expiresIn) { m_expiresIn = expiresIn; }

private:
    
    QString m_accessToken;
    QString m_tokenType;
    QString m_refreshToken;
    QString m_scope;
    int m_expiresIn;
};

#endif // OAUTH2TOKENDTO_H //
