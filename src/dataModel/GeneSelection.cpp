#include "GeneSelection.h"

/*
 *     QString m_id;
    QString m_name;
    QString m_userId;
    QString m_datasetId;
    QString m_featureIds;
    QVector<QString> m_featureIds;
    QString m_type;
    QString m_status;
    QVector<QString> m_oboFroundryTerms;
    QString m_comment;
    */
GeneSelection::GeneSelection()
    : m_id(),
      m_name(),
      m_userId(),
      m_datasetId(),
      m_featureIds(),
      m_type(),
      m_status(),
      m_oboFroundryTerms(),
      m_comment()

{

}

GeneSelection::GeneSelection(const GeneSelection& other)
    : m_id(other.m_id),
      m_name(other.m_name),
      m_userId(other.m_userId),
      m_datasetId(other.m_datasetId),
      m_featureIds(other.m_featureIds),
      m_type(other.m_type),
      m_status(other.m_status),
      m_oboFroundryTerms(other.m_oboFroundryTerms),
      m_comment(other.m_comment)
{

}

GeneSelection::~GeneSelection()
{

}

GeneSelection& GeneSelection::operator=(const GeneSelection& other)
{
    m_id = other.m_id;
    m_name = other.m_name;
    m_userId = other.m_userId;
    m_datasetId = other.m_datasetId;
    m_featureIds = other.m_featureIds;
    m_type = other.m_type;
    m_status = other.m_status;
    m_oboFroundryTerms = other.m_oboFroundryTerms;
    m_comment = other.m_comment;
    return (*this);
}

bool GeneSelection::operator==(const GeneSelection& other) const
{
    return(
                m_id == other.m_id &&
                m_name == other.m_name &&
                m_userId == other.m_userId &&
                m_datasetId == other.m_datasetId &&
                m_featureIds == other.m_featureIds &&
                m_type == other.m_type &&
                m_status == other.m_status &&
                m_oboFroundryTerms == other.m_oboFroundryTerms &&
                m_comment == other.m_comment
        );
}

