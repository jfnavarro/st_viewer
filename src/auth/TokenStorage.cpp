/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "TokenStorage.h"

#include <QString>
#include <QDebug>
#include <QUuid>
#include <QDateTime>

#include "utils/Utils.h"

TokenStorage::TokenStorage(QObject* parent)
    : QObject(parent),
      m_storage(nullptr)
{
    m_storage = new QSettings();
}

TokenStorage::~TokenStorage()
{
    //m_storage is smart pointer
}

void TokenStorage::setAccessToken(const QUuid& accessToken)
{
    Q_ASSERT(!accessToken.isNull() && "[TokenStorage] Trying to set invalid access token!");
    m_storage->setValue(Globals::SettingsAcessToken, accessToken);
}

bool TokenStorage::isExpired() const
{
    QDateTime currentDate = QDateTime::currentDateTimeUtc();
    QDateTime expirationDate = m_storage->value(Globals::SettingsTokenExpiresAt).toDateTime();
    return (currentDate > expirationDate);
}

void TokenStorage::setAccessToken(const QUuid& accessToken, int expiresIn)
{
    Q_ASSERT(!accessToken.isNull() && "[TokenStorage] Trying to set invalid access token!");
    // use UTC time as the reference point
    QDateTime expirationDate = QDateTime::currentDateTimeUtc().addSecs(expiresIn);
    m_storage->setValue(Globals::SettingsAcessToken, accessToken);
    m_storage->setValue(Globals::SettingsTokenExpiresAt, expirationDate);
}

const QUuid TokenStorage::getAccessToken() const
{
    // return stored QUuid or null QUuid if not found
    return m_storage->value(Globals::SettingsAcessToken, QUuid()).toUuid();
}

void TokenStorage::setRefreshToken(const QUuid& refreshToken)
{
    Q_ASSERT(!refreshToken.isNull() && "[TokenStorage] Trying to set invalid refresh token!");
    m_storage->setValue(Globals::SettingsRefreshToken, refreshToken);
}

const QUuid TokenStorage::getRefreshToken() const
{
    // return stored QUuid or null QUuid if not found
    return m_storage->value(Globals::SettingsRefreshToken, QUuid()).toUuid();
}

void TokenStorage::cleanAll()
{
    m_storage->remove(Globals::SettingsAcessToken);
    m_storage->remove(Globals::SettingsTokenExpiresAt);
    m_storage->remove(Globals::SettingsRefreshToken);
}

bool TokenStorage::hasAccessToken() const
{
    return m_storage->contains(Globals::SettingsAcessToken);
}

bool TokenStorage::hasRefreshToken() const
{
    return m_storage->contains(Globals::SettingsRefreshToken);
}
