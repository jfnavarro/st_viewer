/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "Configuration.h"

#include <QDebug>

#include "utils/Utils.h"
#include "SettingsFormatXML.h"



Configuration::Configuration(QObject *parent)
    : QObject(parent), m_settings(0)
{

}

Configuration::~Configuration()
{
    //no need to delete m_settings
}

void Configuration::init()
{
    QSettings::Format format = QSettings::registerFormat(
                                   "conf", &SettingsFormatXML::readXMLFile,
                                   &SettingsFormatXML::writeXMLFile);
    m_settings = QPointer<QSettings>(new QSettings(":/config/application.conf", format, this));
}

void Configuration::finalize()
{

}

const QString Configuration::readSetting(const QString& key) const
{
    // early out
    if (m_settings == 0) {
        return QString();
    }
    m_settings->beginGroup(Globals::SettingsPrefixConfFile);
    QVariant value = m_settings->value(key);
    m_settings->endGroup();
    if (!value.isValid() || !value.canConvert(QVariant::String)) {
        qWarning() << "[Confiuration] Warning: Invalid configuration key:"
                   << (Globals::SettingsPrefixConfFile + SettingsFormatXML::GROUP_DELIMITER +  key);
    }
    return value.toString();
}

const QString Configuration::EndPointUrl() const
{
   return readSetting(QStringLiteral("application/url"));
}

//version
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

const QString Configuration::dataEndpointFeatures() const
{
   return readSetting(QStringLiteral("data/endpoints/feature"));
}

const QString Configuration::dataEndpointGenes() const
{
   return readSetting(QStringLiteral("data/endpoints/gene"));
}

const QString Configuration::dataEndpointHitCounts() const
{
   return readSetting(QStringLiteral("data/endpoints/hitcount"));
}

const QString Configuration::dataEndpointReducedFeatures() const
{
   return readSetting(QStringLiteral("data/endpoints/reducedfeature"));
}

const QString Configuration::dataEndpointUsers() const
{
   return readSetting(QStringLiteral("data/endpoints/user"));
}

const QString Configuration::dataEndpointSelections() const
{
   return readSetting(QStringLiteral("data/endpoints/selections"));
}

const QString Configuration::dataEndpointFigures() const
{
   return readSetting(QStringLiteral("data/endpoints/figure"));
}
