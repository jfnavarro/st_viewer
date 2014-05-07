/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "DownloadManager.h"

#include "network/NetworkReply.h"
#include "error/Error.h"

namespace async
{

DataRequest::DataRequest()
{
}

DataRequest::~DataRequest()
{

}

DataRequest::DataRequest(const DataRequest& other)
{
    m_error_list = other.m_error_list;
    m_return_code = other.return_code();
}

DataRequest& DataRequest::operator=(const DataRequest& other)
{
    m_error_list = other.m_error_list;
    m_return_code = other.return_code();
    return (*this);
}

bool DataRequest::operator==(const DataRequest& other) const
{
    return(m_return_code == other.m_return_code
           && m_error_list == other.m_error_list);
}

DataRequest::Code DataRequest::return_code() const
{
    return m_return_code;
}

void DataRequest::return_code(DataRequest::Code code)
{
    m_return_code = code;
}

void DataRequest::addError(const Error *error)
{
    m_error_list.append(error);
}

QList<const Error*> DataRequest::getErrors() const
{
    return m_error_list;
}
/*
DownloadManager::DownloadManager(async::DataRequest *request, QObject * parent) :
    QObject(parent), m_request(request)
{
    Q_ASSERT_X(m_request, "DownloadManager Error", "DaraRequest is null");
    //Make connnection with DataRequest
    connect(m_request, SIGNAL(signalAbort()), this, SLOT(slotAbort()));
}

DownloadManager::~DownloadManager()
{
    if (!m_request.isNull()) {
        delete m_request;
    }
    m_request = 0;
    //NOTE elements of replies and errors are deleted by their owners
    m_reply_list.clear();
    m_error_list.clear();
}

void DownloadManager::addError(Error * error)
{
    m_error_list.append(error);
}

int DownloadManager::countErrors() const
{
    return m_error_list.count();
}

void DownloadManager::addItem(NetworkReply *item)
{
    m_reply_list.append(item);
}

int DownloadManager::countItems() const
{
    //TODO check if any reply is NULL and remove it to deal with lost connection
    return m_reply_list.count();
}

void DownloadManager::delItem(NetworkReply *item)
{
    m_reply_list.removeOne(item);
}

void DownloadManager::finish()
{
    //TODO check if there are replies left and abort them
    //check for error
    if (m_error_list.count() > 0) {
        m_request->return_code(async::DataRequest::CodeError);
        if (m_error_list.count() > 1) {
            QString errortext;
            foreach(Error * e, m_error_list) {
                errortext += (e->name() + " : " + e->description()) + "\n";
            }
            m_request->slotError(new Error("Multiple Data Error", errortext, this));
        } else {
            Error *error = m_error_list.first();
            m_request->slotError(error);
        }
    } else if (m_request->return_code() != async::DataRequest::CodeAbort) {
        m_request->return_code(async::DataRequest::CodeSuccess);
        m_request->slotFinished();
    } else {
        //do nothing
    }
}

void DownloadManager::slotAbort()
{
    //iterate replys and send abort signal
    m_request->return_code(async::DataRequest::CodeAbort);
    foreach(NetworkReply * reply, m_reply_list) {
        reply->slotAbort(); //this will cause reply to emit signalFinished that will notify dataproxy
    }
    m_request->slotFinished();
}

void DownloadManager::start()
{
    //TODO
    //start timer and connect it to timeOut()
}

void DownloadManager::timedOut()
{
    //TODO
    //abort all the replies
    //m_request->return_code(async::DataRequest::CodeTimedOut);
    //m_request->slotFinished();
}
*/
} // namespace async //
