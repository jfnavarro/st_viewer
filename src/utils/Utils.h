/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef UTILS_H
#define UTILS_H

#include <array>
#include <QString>
#include <QSizeF>
#include <QColor>
#include <QDebug>
#include "options_cmake.h"

// Global defines
namespace Globals
{

/** GRAPHIC PARAMETERS DEFAULT VALUES **/
static const QColor DEFAULT_COLOR_GENE = QColor::fromHsv(0, 190, 255);
static const int GENE_THRESHOLD_MAX = 100;
static const int GENE_THRESHOLD_MIN = 1;
static const qreal GENE_SIZE_DEFAULT = 1.0;
static const qreal GENE_INTENSITY_DEFAULT = 1.0;
static const qreal GENE_SHINNE_DEFAULT = 1.0;
static const QColor DEFAULT_COLOR_GRID_BORDER = Qt::darkRed;
static const QColor DEFAULT_COLOR_GRID = Qt::darkGreen;

enum GeneVisualMode {
    NormalMode = 0,
    DynamicRangeMode = 1,
    HeatMapMode = 2
};

enum GeneShape  {
    Circle = 0,
    Cross = 1,
    Square = 2
};

enum Anchor
{
    Center = 1,
    North = 2,
    NorthEast = 3,
    East = 4,
    SouthEast = 5,
    South = 6,
    SouthWest = 7,
    West = 8,
    NorthWest = 9,
    None = 10
};

/** GRAPHICAL ITEMS ANCHOR POSITIONS **/
static const Anchor DEFAULT_ANCHOR_MINIMAP = SouthEast;
static const Anchor DEFAULT_ANCHOR_LEGEND = NorthEast;
static const Anchor DEFAULT_ANCHOR_GRID = None;
static const Anchor DEFAULT_ANCHOR_IMAGE = None;
static const Anchor DEFAULT_ANCHOR_GENE = None;

/** VERSION **/
static const qulonglong MAJOR = VERSION_MAJOR;
static const qulonglong MINOR = VERSION_MINOR;
static const qulonglong PATCH = VERSION_REVISION;
static const std::array< qulonglong, 3> VersionNumbers = { MAJOR, MINOR, PATCH };
static const QString VERSION = QString("%1.%2.%3").arg(MAJOR).arg(MINOR).arg(PATCH);

/** SETTINGS **/
static const QString SettingsPrefixConfFile = QStringLiteral("configuration");
static const QString SettingsGlobal = QStringLiteral("Global");
static const QString SettingsLocale = QStringLiteral("Locale");
static const QString SettingsGeometry = QStringLiteral("Geometry");
static const QString SettingsState = QStringLiteral("State");
static const QString SettingsUsers = QStringLiteral("Users");
static const QString SettingsToken = QStringLiteral("TokenStorage");
static const QString SettingsAcessToken = QStringLiteral("access_token");
static const QString SettingsRefreshToken = QStringLiteral("refresh_token");
static const QString SettingsGeneColor = QStringLiteral("GeneColor");
static const QString SettingsGeneSize = QStringLiteral("GeneSize");
static const QString SettingsGeneShape = QStringLiteral("GeneShape");
static const QString SettingsGeneIntensity = QStringLiteral("GeneIntensity");
static const QString SettingsGridColor = QStringLiteral("GridColor");
static const QString SettingsTokenExpiresAt = QStringLiteral("expires_at");

/** NETWORK **/
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

static const QString ROLE_USER = QStringLiteral("ROLE_USER");
static const QString ROLE_CM = QStringLiteral("ROLE_CM");

enum HttpRequestType {
    HttpRequestTypeNone = 0,
    HttpRequestTypeGet = 1,
    HttpRequestTypePost = 2,
    HttpRequestTypePut = 3,
    HttpRequestTypeDelete = 4
};

}

#endif // UTILS_H
