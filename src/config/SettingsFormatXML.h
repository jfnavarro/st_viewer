/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef SETTINGSFORMATXML_H
#define SETTINGSFORMATXML_H

#include <QChar>
#include <QIODevice>
#include <QSettings>

// Import/Export plug-in for saving settings data in an XML format.

// TODO replace for namespace and remove the class
class SettingsFormatXML
{

public:
    static const QChar GROUP_DELIMITER;

    static bool writeXMLFile(QIODevice& device, const QSettings::SettingsMap& map);
    static bool readXMLFile(QIODevice& device, QSettings::SettingsMap& map);
};

#endif // SETTINGSFORMATXML_H //
