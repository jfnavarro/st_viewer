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
#include <QDateTime>
#include <QColor>

#include <QDebug>
#include "options_cmake.h"

// Global defines
//TODO Defines that are not used globally should be defined in the scope they are used.
namespace Globals
{

/*** version ***/
static const qulonglong MAJOR = VERSION_MAJOR;
static const qulonglong MINOR = VERSION_MINOR;
static const qulonglong PATCH = VERSION_REVISION;
static const std::array< qulonglong, 3> VersionNumbers{{ VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION }};
static const QString VERSION = QString("%1.%2.%3").arg(MAJOR).arg(MINOR).arg(PATCH);

/**** setttings ****/
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
/**** setttings ****/


/* configuration for rendering */
//TODO move these guys to an external class that will implement save/load from qsettins
static const QColor color_grid = Qt::darkGreen;
static const QColor color_grid_border = Qt::darkRed;
static const QColor color_gene = QColor::fromHsv(0, 190, 255);
static const QColor minimap_view_color = Qt::blue;
static const QColor minimap_scene_color = Qt::red;

static const qreal minimap_height = 100;
static const qreal minimap_width = 100;
static const qreal heatmap_height = 150;
static const qreal heatmap_width = 25;
static const qreal heatmap_bar_width = 35;

enum ThresholdMode {
    IndividualGeneMode = 0,
    GlobalGeneMode = 1
};

enum VisualMode {
    NormalMode = 0,
    DynamicRangeMode = 1,
    HeatMapMode = 2
};

enum Shape {
    Circle = 0,
    Cross = 1,
    Square = 2
};

static const Shape gene_shape = Circle;
static const qreal gene_size = 1.0f;
static const qreal gene_intensity = 1.0f;
static const int gene_lower_limit = 1;
static const int gene_upper_limit = std::numeric_limits<int>::max();

static const int gene_threshold_max = 100;
static const int gene_threshold_min = 1;

static const qreal grid_line_size = 1;
static const qreal grid_line_space = 5;

static const int gene_intensity_min = 1;
static const int gene_intensity_max = 10;

static const int gene_size_min = 10;
static const int gene_size_max = 30;

#if defined(Q_OS_WIN)
static const QString PluginPrefix = "";
static const QString PluginExtension = ".dll";
#elif defined(Q_OS_LINUX)
static const QString PluginPrefix = "lib";
static const QString PluginExtension = ".so";
#elif defined(Q_OS_MAC)
static const QString PluginPrefix = "lib";
static const QString PluginExtension = ".dylib";
#else
#error Unsupported platform
#endif


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

static const QString TEMP_PREFIX = QStringLiteral("stvi_temp_XXXXXX_");
static const QString RESTORE_FILE = QStringLiteral("stvi_filemap");


static const QString ContentTypeNONE = QStringLiteral("");
static const QString ContentTypeJSON = QStringLiteral("application/json");
static const QString ContentTypeJPEG = QStringLiteral("image/jpeg");

typedef enum {HttpRequestTypeNone, HttpRequestTypeGet, HttpRequestTypePost, HttpRequestTypePut} HttpRequestType;

static const QString PARAM_TYPE = QStringLiteral("type");
static const QString PARAM_DATASET = QStringLiteral("dataset");
static const QString PARAM_GENE = QStringLiteral("gene");
static const QString PARAM_FILE = QStringLiteral("file");

static const QString ROLE_USER = QStringLiteral("ROLE_USER");
static const QString ROLE_CM = QStringLiteral("ROLE_CM");

static const QSizeF DEFAULT_ZOOM_MIN = QSizeF(1.00, 1.00);
static const QSizeF DEFAULT_ZOOM_MAX = QSizeF(20.0, 20.0);
static const QSizeF DEFAULT_ZOOM = DEFAULT_ZOOM_MIN;

static const qreal DEFAULT_ZOOM_IN  = qreal(1.1 / 1.0);
static const qreal DEFAULT_ZOOM_OUT = qreal(1.0 / 1.1);

static const QSize DEFAULT_BOUND_SIZE = QSize(100, 100);
}

// some useful global functions
namespace Utils
{
    
QDateTime decodeJsonDateTime(const QString &jsonDateTime);
void char2hex(QChar dec, QString &str);
QString urlEncode(const QByteArray &c);
QString urlEncode(const QString &c);
QString formatStorage(qlonglong storageSpace);
size_t getMemorySize();

}

#endif // UTILS_H
