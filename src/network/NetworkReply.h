/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef NETWORKREPLY_H
#define NETWORKREPLY_H

#include <QObject>
#include <QNetworkReply>
#include <QPointer>
#include <QList>

class Error;
class QSslError;
class QJsonDocument;

// ContentType provides convenience functionality for managing content-type
// information. Currently it only supports parsing and extracting mime-data.
class ContentType : public QObject
{
    Q_OBJECT
public:

    explicit ContentType(QObject* parent = 0);
    ContentType(const QString& contentType, QObject* parent = 0);
    virtual ~ContentType();

     const QString& mime() const { return m_mime; }

    void header(const QString& value);

private:

    QString m_mime;
};

// NetworkReply represents a handle to the asynchronous network request
// managed by the NetworkManager. This handle provides a means of parsing
// the resulting data as well as listening to any errors.
// NetworkReply extends AsyncRequest which means that it is guaranteed to
// emit a signalFinished when it has completed.
class NetworkReply: public QObject
{
    Q_OBJECT

public:

    enum ReturnCode {
        CodeSuccess = 0x01,
        CodeAbort = 0x02,
        CodeError = 0x04
    };
    Q_DECLARE_FLAGS(ReturnCodes, ReturnCode)

    typedef QList<Error*> ErrorList;

    explicit NetworkReply(QNetworkReply* networkReply = 0, QObject* parent = 0);
    virtual ~NetworkReply();

    // user data
     const QVariant customData() const;
     void setCustomData(QVariant data);

    // parse body
    QJsonDocument getJSON();
    QString getText() const;
    QByteArray getRaw() const;

     const ContentType* contentType() const;
     bool isType(const QString& mime) const;

     bool isFinished() const;
     bool hasErrors() const;

     const NetworkReply::ErrorList& errors() const;

public slots:

    void slotAbort();
    void slotFinished();
    void slotMetaDataChanged();
    void slotError(QNetworkReply::NetworkError networkError);
    void slotSslErrors(QList<QSslError> sslErrorList);

signals:
    // signal operation Finished (code = abort, error, ok)
    void signalFinished(QVariant code, QVariant data);

private:

     void registerError(Error* error);

    // QT network reply
    QScopedPointer<QNetworkReply> m_reply;
    // derived data
    mutable ContentType *m_contentType;
    // errors
    ErrorList m_errors;
    // custom data
    QVariant m_data;

    Q_DISABLE_COPY(NetworkReply)
};

#endif // NETWORKREPLY_H //
