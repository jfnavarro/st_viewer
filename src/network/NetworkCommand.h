/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef NETWORKCOMMAND_H
#define NETWORKCOMMAND_H

#include <QObject>
#include <QUrl>
#include <QUrlQuery>

#include "utils/Utils.h"

class QString;

// The network command is an abstraction of a single network request command.
// It encompasses the target URL, query items as well as indicates the type of
// request.
class NetworkCommand : public QObject
{
    Q_OBJECT

public:

    explicit NetworkCommand(QObject *parent = 0);
    NetworkCommand(const QUrl& url,
                   Globals::HttpRequestType type = Globals::HttpRequestTypeNone,
                   QObject *parent = 0);
    virtual ~NetworkCommand();

    // member access
    const QUrl& url() const;
    Globals::HttpRequestType type() const;
    const QUrlQuery& query() const;

    // convenience wrapper functions to add parameters to the network command
    void addQueryItem(const QString &param, const QString &value = QString());
    void addQueryItem(const QString &param, const int value);
    void addQueryItem(const QString &param, const qreal value);

    // adds query items from qobject meta data if its elements are possible to parse to QVariant
    void addQueryItems(QObject *object);

    // returns the current query item for the param given
    const QString getQueryItem(const QString& param) const;

    // returns the query part fully encoded.
    const QString getEncodedQuery() const;

    // returns the URL part fully encoded. Note this includes the URL query.
    const QString getEncodedUrl() const;

    // sets the body as a QByteArray
    // usually needed for sending JSON objects (used in PUT/POST)
    void setBody(const QByteArray &body);
    const QByteArray body() const;

private:

    typedef Globals::HttpRequestType Type;
    QUrl m_url;
    Type m_type;
    QUrlQuery m_query;
    QByteArray m_body;

    Q_DISABLE_COPY(NetworkCommand)
};

#endif // NETWORKCOMMAND_H //
