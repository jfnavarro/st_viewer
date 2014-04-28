/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef NETWORKCOMMAND_H
#define NETWORKCOMMAND_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QUrlQuery>
#include "utils/Utils.h"


// The network command is an abstraction of a single network request command.
// It encompasses the target URL, query items as well as indicates the type of
// request.
class NetworkCommand : public QObject
{
    Q_OBJECT

public:

    explicit NetworkCommand(QObject* parent = 0);

    explicit NetworkCommand(const QUrl& url,
                            Globals::HttpRequestType type = Globals::HttpRequestTypeNone,
                            QObject* parent = 0);

    virtual ~NetworkCommand();

    // member access
    const QUrl& url() const { return m_url; }
    Globals::HttpRequestType type() const { return m_type; }
    const QUrlQuery& query() const { return m_query; }

    // convenience wrapper functions
    void addQueryItem(const QString& param, const QString& value = QString());
    void addQueryItem(const QString& param, int value);
    void addQueryItem(const QString& param, double value);
    void addQueryItems(QObject* object); // adds query items from qobject meta data

    const QString getQueryItem(const QString& param) const;

    // returns the query part fully encoded.
    const QString getEncodedQuery() const;

    // returns the URL part fully encoded. Note this includes the URL query.
    const QString getEncodedUrl() const;

private:

    typedef Globals::HttpRequestType Type;
    QUrl m_url;
    Type m_type;
    QUrlQuery m_query;

    Q_DISABLE_COPY(NetworkCommand)
};

#endif // NETWORKCOMMAND_H //
