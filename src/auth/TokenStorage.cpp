#include "TokenStorage.h"

#include <QString>
#include <QDebug>
#include <QUuid>
#include <QDateTime>

#include "SettingsNetwork.h"
using namespace Network;

TokenStorage::TokenStorage()
    : m_storage(new QSettings())
{
}

TokenStorage::~TokenStorage()
{
}

void TokenStorage::setAccessToken(const QUuid &accessToken)
{
    Q_ASSERT(!accessToken.isNull());
    m_storage->setValue(SettingsAcessToken, accessToken);
}

bool TokenStorage::isExpired() const
{
    const QDateTime currentDate = QDateTime::currentDateTimeUtc();
    const QDateTime expirationDate = m_storage->value(SettingsTokenExpiresAt).toDateTime();
    return currentDate > expirationDate;
}

void TokenStorage::setAccessToken(const QUuid &accessToken, int expiresIn)
{
    Q_ASSERT(!accessToken.isNull());
    // Use UTC time as the reference point
    const QDateTime expirationDate = QDateTime::currentDateTimeUtc().addSecs(expiresIn);
    m_storage->setValue(SettingsAcessToken, accessToken);
    m_storage->setValue(SettingsTokenExpiresAt, expirationDate);
}

const QUuid TokenStorage::getAccessToken() const
{
    // Return stored QUuid or null QUuid if not found
    return m_storage->value(SettingsAcessToken, QUuid()).toUuid();
}

void TokenStorage::setRefreshToken(const QUuid &refreshToken)
{
    Q_ASSERT(!refreshToken.isNull());
    m_storage->setValue(SettingsRefreshToken, refreshToken);
}

const QUuid TokenStorage::getRefreshToken() const
{
    // Return stored QUuid or null QUuid if not found
    return m_storage->value(SettingsRefreshToken, QUuid()).toUuid();
}

void TokenStorage::cleanAll()
{
    m_storage->remove(SettingsAcessToken);
    m_storage->remove(SettingsTokenExpiresAt);
    m_storage->remove(SettingsRefreshToken);
}

bool TokenStorage::hasAccessToken() const
{
    return m_storage->contains(SettingsAcessToken);
}

bool TokenStorage::hasRefreshToken() const
{
    return m_storage->contains(SettingsRefreshToken);
}
