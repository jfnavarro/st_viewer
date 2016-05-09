#ifndef TOKENSTORAGE_H
#define TOKENSTORAGE_H

#include <QSettings>

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

    // Setters
    void setAccessToken(const QUuid &accessToken);
    void setAccessToken(const QUuid &accessToken, int expiresIn);
    void setRefreshToken(const QUuid &refreshToken);

    // If the access token has expired or not
    bool isExpired() const;

    // Getters
    const QUuid getAccessToken() const;
    const QUuid getRefreshToken() const;

    // To check if the user has already an access token or refresh token
    bool hasAccessToken() const;
    bool hasRefreshToken() const;

    // Cleans the content of the accesstoken
    void cleanAll();

private:
    QScopedPointer<QSettings> m_storage;

    Q_DISABLE_COPY(TokenStorage)
};

#endif /* TOKENSTORAGE_H */
