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
    
    NetworkCommand(const QUrl& url, Globals::HttpRequestType type = Globals::HttpRequestTypeNone,
                   QObject* parent = 0);
    
    virtual ~NetworkCommand();

    // member access
    inline const QUrl& url() const { return m_url; }
    inline Globals::HttpRequestType type() const { return m_type; }
    inline const QUrlQuery& query() const { return m_query; }

    // convenience wrapper functions
    inline void addQueryItem(const QString& param,const QString& value = QString())
                            { m_query.addQueryItem(param, value); }
                            
    inline void addQueryItem(const QString& param,int value)
                            { m_query.addQueryItem(param, QString::number(value)); }
                            
    inline void addQueryItem(const QString& param, double value)
                            { m_query.addQueryItem(param, QString::number(value)); }
                             
    void addQueryItems(QObject* object); // adds query items from qobject meta data

    inline const QString getQueryItem(const QString& param) const { return m_query.queryItemValue(param); }

    // returns the query part fully encoded.
    inline const QString getEncodedQuery() const { return m_query.query(QUrl::FullyEncoded); }
    // returns the URL part fully encoded. Note this includes the URL query.
    const QString getEncodedUrl() const; 
         
private:
  
    typedef Globals::HttpRequestType Type;
    QUrl m_url;
    Type m_type;
    QUrlQuery m_query;
};

#endif // NETWORKCOMMAND_H //
