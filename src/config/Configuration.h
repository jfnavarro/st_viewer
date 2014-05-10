/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QSettings>
#include <QString>
#include <QPointer>

#include "utils/Singleton.h"

// Configuration is a convenience class that simplifies the access of
// statically stored configuration data by providing a globally
// accessible instance.
class Configuration : public QObject, public Singleton<Configuration>
{
    Q_OBJECT
public:

    explicit Configuration(QObject *parent = 0);
    virtual ~Configuration();

    // singleton initialize/finalize
    void init();
    void finalize();

public:

    //TODO if more settings are added let each object accessing the
    //     configurations store the access string literals as static values
    //     and make readSetting public instead. doesn't make sense to provide
    //     each setting as a function.

    //url
    const QString EndPointUrl() const;

    //version
    const QString dataEndpointMinVersion() const;

    // oauth
    const QString oauthClientID() const;
    const QString oauthScope() const;
    const QString oauthSecret() const;
    const QString oauthEndpointAuthorize() const;
    const QString oauthEndpointToken() const;

    // data access
    const QString dataEndpointChips() const;
    const QString dataEndpointDatasets() const;
    const QString dataEndpointImageAlingment() const;
    const QString dataEndpointFeatures() const;
    const QString dataEndpointGenes() const;
    const QString dataEndpointUsers() const;
    const QString dataEndpointSelections() const;
    const QString dataEndpointFigures() const;

private:

    const QString readSetting(const QString& key) const;

    QPointer<QSettings> m_settings;

    Q_DISABLE_COPY(Configuration)
};

#endif  /* CONFIGURATION_H */
