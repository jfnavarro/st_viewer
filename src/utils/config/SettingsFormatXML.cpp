/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "SettingsFormatXML.h"

#include <QXmlStreamReader>
#include <QStringList>
#include <QDir>
#include <QDebug>
#include "utils/DebugHelper.h"

const QChar SettingsFormatXML::GROUP_DELIMITER = QDir::separator();

bool SettingsFormatXML::readXMLFile(QIODevice& device, QSettings::SettingsMap& map)
{
    QXmlStreamReader xmlReader(&device);
    QStringList elements;
    // read until end is reached or error occurs
    while (!xmlReader.atEnd() && !xmlReader.hasError()) {
        // get next token
        xmlReader.readNext();
        // if node push it to element list
        if (xmlReader.isStartElement()) {
            elements.append(xmlReader.name().toString());
            // if end node pop
        } else if (xmlReader.isEndElement()) {
            if (!elements.isEmpty()) elements.removeLast();
            // if it is some data (excl. whitespaces) parse it
        } else if (xmlReader.isCharacters() && !xmlReader.isWhitespace()) {
            QString key = elements.join('/');
            map[key] = xmlReader.text().toString();
        }
    }
    // show warning on error
    if (xmlReader.hasError()) {
        qWarning() << xmlReader.errorString();
        return false;
    }
    return true;
}

bool SettingsFormatXML::writeXMLFile(QIODevice& device, const QSettings::SettingsMap& map)
{
    //NOTE: implement if needed
    // check "http://www.openshots.de/2011/03/qsettings-mit-xml-format/" for example
    return true;
}
