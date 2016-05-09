#ifndef SETTINGSNETWORK_H
#define SETTINGSNETWORK_H

#include <QString>

// This header file simply contains some common configuration
// structures for the Network related classes

namespace Network
{
enum HttpRequestType {
    HttpRequestTypeNone = 0,
    HttpRequestTypeGet = 1,
    HttpRequestTypePost = 2,
    HttpRequestTypePut = 3,
    HttpRequestTypeDelete = 4
};

/** NETWORK CONFIGURATION **/
static const QString LBL_ACCESS_TOKEN_USERNAME = QStringLiteral("username");
static const QString LBL_ACCESS_TOKEN_PASSWORD = QStringLiteral("password");
static const QString LBL_GRANT_TYPE = QStringLiteral("grant_type");
static const QString LBL_GRANT_PASSWORD = QStringLiteral("password");
static const QString LBL_SCOPE = QStringLiteral("scope");
static const QString LBL_ERROR_DESCRIPTION = QStringLiteral("error_description");
static const QString LBL_RESPONSE_TYPE = QStringLiteral("response_type");
static const QString LBL_APPROVAL_PROMPT = QStringLiteral("approval_prompt");
static const QString LBL_FORCE = QStringLiteral("force");
static const QString LBL_ACCESS_TYPE = QStringLiteral("access_type");
static const QString LBL_OFFLINE = QStringLiteral("offline");

static const QString ContentTypeNONE = QStringLiteral("");
static const QString ContentTypeJSON = QStringLiteral("application/json");
static const QString ContentTypeJPEG = QStringLiteral("image/jpeg");

static const QString PARAM_TYPE = QStringLiteral("type");
static const QString PARAM_DATASET = QStringLiteral("dataset");
static const QString PARAM_GENE = QStringLiteral("gene");
static const QString PARAM_FILE = QStringLiteral("file");
static const QString PARAM_SELECTION = QStringLiteral("selection");

static const QString SettingsTokenExpiresAt = QStringLiteral("expires_at");
static const QString SettingsAcessToken = QStringLiteral("access_token");
static const QString SettingsRefreshToken = QStringLiteral("refresh_token");
}
#endif // SETTINGSNETWORK_H
