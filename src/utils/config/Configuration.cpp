/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QDebug>
#include "utils/DebugHelper.h"

#include "utils/Utils.h"

#include "SettingsFormatXML.h"

#include "Configuration.h"

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
    if (m_settings == 0)
    {
        return QString();
    }

    m_settings->beginGroup(Globals::SettingsPrefixConfFile);
    QVariant value = m_settings->value(key);
    m_settings->endGroup();
    
    if (!value.isValid() || !value.canConvert(QVariant::String)) 
    {
        qWarning() << "[Confiuration] Warning: Invalid configuration key:"
        << (Globals::SettingsPrefixConfFile + SettingsFormatXML::GROUP_DELIMITER +  key);
    }
    return value.toString();
}
