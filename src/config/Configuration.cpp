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

bool Configuration::is_valid() const
{
    return !m_settings.isNull() && m_settings->status() == QSettings::NoError;
}

