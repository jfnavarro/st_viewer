#include "Configuration.h"
#include <QDebug>
#include "SettingsFormatXML.h"
#include "options_cmake.h"
#include <QLibraryInfo>
#include <QApplication>
#include <QDir>

static const QString SettingsPrefixConfFile = QStringLiteral("configuration");

Configuration::Configuration()
    : m_settings(nullptr)
{
    QSettings::Format format = QSettings::registerFormat("conf",
                                                         &SettingsFormatXML::readXMLFile,
                                                         &SettingsFormatXML::writeXMLFile);
    QDir dir(QApplication::applicationDirPath());
#if defined Q_OS_MAC
    dir.cdUp();
    dir.cd("Resources");
#endif
    const QString config_file = dir.canonicalPath() + QDir::separator() + CONFIG_FILE;
    m_settings.reset(new QSettings(config_file, format, nullptr));
}

Configuration::~Configuration()
{
}

const QString Configuration::readSetting(const QString &key) const
{
    // early out
    if (!is_valid()) {
        return QString();
    }

    m_settings->beginGroup(SettingsPrefixConfFile);
    const QVariant value = m_settings->value(key);
    m_settings->endGroup();
    if (!value.isValid() || !value.canConvert(QVariant::String)) {
        qDebug() << "[Configuration] Warning: Invalid configuration key:"
                 << (SettingsPrefixConfFile + SettingsFormatXML::GROUP_DELIMITER + key);
        return QString();
    }

    return value.toString();
}

const QString Configuration::EndPointUrl() const
{
    return readSetting(QStringLiteral("application/url"));
}

// version
const QString Configuration::dataEndpointMinVersion() const
{
    return readSetting(QStringLiteral("data/endpoints/version"));
}

// oauth
const QString Configuration::oauthClientID() const
{
    return readSetting(QStringLiteral("oauth/clientid"));
}

const QString Configuration::oauthScope() const
{
    return readSetting(QStringLiteral("oauth/scope"));
}

const QString Configuration::oauthSecret() const
{
    return readSetting(QStringLiteral("oauth/secret"));
}

const QString Configuration::oauthEndpointAuthorize() const
{
    return readSetting(QStringLiteral("oauth/endpoints/authorize"));
}

const QString Configuration::oauthEndpointToken() const
{
    return readSetting(QStringLiteral("oauth/endpoints/token"));
}

// data access
const QString Configuration::dataEndpointChips() const
{
    return readSetting(QStringLiteral("data/endpoints/chip"));
}

const QString Configuration::dataEndpointDatasets() const
{
    return readSetting(QStringLiteral("data/endpoints/dataset"));
}

const QString Configuration::dataEndpointImageAlingment() const
{
    return readSetting(QStringLiteral("data/endpoints/imagealignment"));
}

const QString Configuration::dataEndpointFeatures() const
{
    return readSetting(QStringLiteral("data/endpoints/features"));
}

const QString Configuration::dataEndpointUsers() const
{
    return readSetting(QStringLiteral("data/endpoints/user"));
}

const QString Configuration::dataEndpointSelections() const
{
    return readSetting(QStringLiteral("data/endpoints/selection"));
}

const QString Configuration::dataEndpointFigures() const
{
    return readSetting(QStringLiteral("data/endpoints/figure"));
}

bool Configuration::is_valid() const
{
    return !m_settings.isNull() && m_settings->status() == QSettings::NoError;
}

bool Configuration::has_network() const
{
    return !readSetting(QStringLiteral("application/url")).trimmed().isEmpty();
}

const QString Configuration::pathSSL() const
{
    QDir dir(QApplication::applicationDirPath());
#if defined Q_OS_MAC
    dir.cdUp();
    dir.cd("Resources");
#endif
    const QString ssl_file = dir.canonicalPath()
            + QDir::separator() + readSetting(QStringLiteral("application/ssl"));
    return ssl_file;
}
