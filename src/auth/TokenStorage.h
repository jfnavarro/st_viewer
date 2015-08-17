/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef TOKENSTORAGE_H
#define TOKENSTORAGE_H

#include <QSettings>
#include <QPointer>
#include "utils/Utils.h"

class QUuid;
class QString;

// TokenStorage provides a data model for authentication data.
// it is based on QSettings to store access token and refresh token
// information
class TokenStorage
{
public:

    TokenStorage();
    ~TokenStorage();

    // setters
    void setAccessToken(const QUuid& accessToken);
    void setAccessToken(const QUuid& accessToken, int expiresIn);
    void setRefreshToken(const QUuid& refreshToken);

    // if the access token has expired or not
    bool isExpired() const;

    // getters
    const QUuid getAccessToken() const;
    const QUuid getRefreshToken() const;

    // to check if the user has already an access token or refresh token
    bool hasAccessToken() const;
    bool hasRefreshToken() const;

    // cleans the content of the accesstoken
    void cleanAll();

private:

    QPointer<QSettings> m_storage;

    Q_DISABLE_COPY(TokenStorage)
};

#endif  /* TOKENSTORAGE_H */

