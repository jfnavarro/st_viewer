/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/


#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QPointer>
#include <QObject>

class Error;
class NetworkReply;

namespace async
{

//NOTE for now DataRequest is synchronous,
//it could be easily be made async

// this object is a simple wrapper for the status of network replies to handle
// asynchronus/syncrhonous downloads and abort operations
class DataRequest
{
public:

    //TODO make it Q_OBJECT and add it a slot/signal to allow to
    //abort downloads from user(progress bar dialog) or timer

    enum Code {
        CodeSuccess = 0x01,
        CodeAbort = 0x02,
        CodeError = 0x04,
        CodeTimedOut = 0x08,
        CodePresent = 0x16
    };

    DataRequest();
    DataRequest(const DataRequest::Code &code);
    DataRequest(const DataRequest& other);
    ~DataRequest();

    DataRequest& operator=(const DataRequest& other);
    bool operator==(const DataRequest& other) const;

    DataRequest::Code return_code() const;
    void return_code(DataRequest::Code flag);

    void addError(QSharedPointer<Error> error);
    QList<QSharedPointer<Error>> getErrors() const;

    bool isSuccessFul() const;

private:

    QList<QSharedPointer<Error>> m_error_list;
    DataRequest::Code m_return_code;

};

} // namespace async //

#endif  /* // DOWNLOADMANAGER_H */
