/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef CONFIGURATION_H
#define	CONFIGURATION_H

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
    inline const QString EndPointUrl() const { return readSetting(QStringLiteral("application/url")); };
    
    //version
    inline const QString dataEndpointMinVersion() const { return readSetting(QStringLiteral("data/endpoints/version")); }
    
    // oauth
    inline const QString oauthClientID() const { return readSetting(QStringLiteral("oauth/clientid")); }
    inline const QString oauthScope() const { return readSetting(QStringLiteral("oauth/scope")); }
    inline const QString oauthSecret() const { return readSetting(QStringLiteral("oauth/secret")); }

    inline const QString oauthEndpointAuthorize() const { return readSetting(QStringLiteral("oauth/endpoints/authorize")); }
    inline const QString oauthEndpointToken() const { return readSetting(QStringLiteral("oauth/endpoints/token")); }

    // data access
    inline const QString dataEndpointChips() const { return readSetting(QStringLiteral("data/endpoints/chip")); }
    inline const QString dataEndpointDatasets() const { return readSetting(QStringLiteral("data/endpoints/dataset")); }
    inline const QString dataEndpointFeatures() const { return readSetting(QStringLiteral("data/endpoints/feature")); }
    inline const QString dataEndpointGenes() const { return readSetting(QStringLiteral("data/endpoints/gene")); }
    inline const QString dataEndpointHitCounts() const { return readSetting(QStringLiteral("data/endpoints/hitcount")); }
    inline const QString dataEndpointReducedFeatures() const { return readSetting(QStringLiteral("data/endpoints/reducedfeature")); }
    inline const QString dataEndpointUsers() const { return readSetting(QStringLiteral("data/endpoints/user")); }
    inline const QString dataEndpointFigures() const { return readSetting(QStringLiteral("data/endpoints/figure")); }

private:

    const QString readSetting(const QString& key) const;

    QPointer<QSettings> m_settings;
};

#endif	/* CONFIGURATION_H */
