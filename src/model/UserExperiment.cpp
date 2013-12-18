/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "UserExperiment.h"

UserExperiment::UserExperiment()
    : m_datasetId(),
      m_userId(),
      m_type(),
      m_inputData(),
      m_outputData()
{

}

UserExperiment::UserExperiment(const QString& datasetId, const QString& userId, const QString& type,
                               const QString& inputData, const QString& outputData)
    : m_datasetId(datasetId), m_userId(userId), m_type(type),
      m_inputData(inputData), m_outputData(outputData)
{

}

UserExperiment::UserExperiment(const UserExperiment& other)
{
    m_datasetId = other.m_datasetId;
    m_userId = other.m_userId;
    m_type = other.m_type;
    m_inputData  = other.m_inputData;
    m_outputData = other.m_outputData;
}

UserExperiment::~UserExperiment()
{

}

UserExperiment& UserExperiment::operator=(const UserExperiment& other)
{
    m_datasetId = other.m_datasetId;
    m_userId = other.m_userId;
    m_type = other.m_type;
    m_inputData = other.m_inputData;
    m_outputData = other.m_outputData;

    return (*this);
}

bool UserExperiment::operator==(const UserExperiment& other) const
{
    return (
            m_datasetId == other.m_datasetId &&
            m_userId == other.m_userId &&
            m_type == other.m_type &&
            m_inputData == other.m_inputData &&
            m_outputData == other.m_outputData
        );
}
