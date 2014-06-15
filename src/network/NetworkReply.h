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

    typedef QList<QSharedPointer<Error>> ErrorList;

    explicit NetworkReply(QNetworkReply *networkReply = 0);
    ~NetworkReply();

    // user data
    const QVariant customData() const;
    void setCustomData(QVariant data);

    // parse body (once parsed data cannot be parsed again)
    QJsonDocument getJSON();
    QString getText() const;
    QByteArray getRaw() const;

    //reply status
    bool isFinished() const;
    bool hasErrors() const;
    ReturnCode return_code() const;

    //reply errors
    const NetworkReply::ErrorList& errors() const;
    QSharedPointer<Error> parseErrors();

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

    void registerError(QSharedPointer<Error> error);

    // Qt network reply
    //TODO consider make it a smart pointer
    QNetworkReply *m_reply;
    // errors
    ErrorList m_errors;
    // custom data
    QVariant m_data;
    // return status code
    ReturnCode m_code;
    // header content type
    QString m_mime;

    Q_DISABLE_COPY(NetworkReply)
};

#endif // NETWORKREPLY_H //
