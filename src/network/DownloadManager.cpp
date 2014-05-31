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

void DataRequest::addError(QSharedPointer<Error> error)
{
    m_error_list.append(error);
}

QList<QSharedPointer<Error>> DataRequest::getErrors() const
{
    return m_error_list;
}

} // namespace async //
