#include "Configuration.h"
#include <QDebug>
#include "options_cmake.h"
#include <QLibraryInfo>
#include <QApplication>
#include <QDir>

static const QString SettingsPrefixConfFile = QStringLiteral("configuration");

Configuration::Configuration()
    : m_settings(nullptr)
{
    QDir dir(QApplication::applicationDirPath());
#if defined Q_OS_MAC
    dir.cdUp();
    dir.cd("Resources");
#endif
    const QString config_file = dir.canonicalPath() + QDir::separator() + CONFIG_FILE;
    m_settings.reset(new QSettings(config_file, nullptr));
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
        qDebug() << "Warning: Invalid configuration key: " << key;
        return QString();
    }

    return value.toString();
}

bool Configuration::is_valid() const
{
    return !m_settings.isNull() && m_settings->status() == QSettings::NoError;
}

