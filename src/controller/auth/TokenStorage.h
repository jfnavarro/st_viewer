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
class TokenStorage : public QObject
{
    Q_OBJECT

public:

    explicit TokenStorage(QObject* parent = 0);
    virtual ~TokenStorage();

    void setAccessToken(const QUuid& accessToken);
    void setAccessToken(const QUuid& accessToken, int expiresIn);

    void setRefreshToken(const QUuid& refreshToken);

    const bool isExpired() const;
    const QUuid getAccessToken() const;
    const QUuid getRefreshToken() const;

    inline const bool hasAccessToken() const
    {
        return m_storage->contains(Globals::SettingsAcessToken);
    }
    inline const bool hasRefreshToken() const
    {
        return m_storage->contains(Globals::SettingsRefreshToken);
    }

    //cleans the content of the accesstoken
    void cleanAll();

private:

    QScopedPointer<QSettings> m_storage;
};

#endif  /* TOKENSTORAGE_H */

