#include "GeneSelection.h"

#include <QMap>
#include <QDate>

SelectionType::SelectionType() :
    name(),
    reads(0),
    normalizedReads(0),
    pixeIntensity(0),
    count(0)
{

}

SelectionType::SelectionType(QString name, int reads,
                                            qreal normalizedReads, qreal pixeIntensity)
    : name(name),
      reads(reads),
      normalizedReads(normalizedReads),
      pixeIntensity(pixeIntensity),
      count(0)
{

}

SelectionType& SelectionType::operator= (const SelectionType& other)
{
    name = other.name;
    reads = other.reads;
    normalizedReads = other.normalizedReads;
    pixeIntensity = other.pixeIntensity;
    count = other.count;
    return (*this);
}

bool SelectionType::operator== (const SelectionType& other) const
{
    return( name == other.name &&
              reads == other.reads &&
              normalizedReads == other.normalizedReads &&
              pixeIntensity == other.pixeIntensity &&
              count == other.count
        );
}

GeneSelection::GeneSelection()
    : m_id(),
      m_name(),
      m_userId(),
      m_datasetId(),
      m_selectedItems(),
      m_type(),
      m_status(),
      m_oboFroundryTerms(),
      m_comment(),
      m_enabled(false),
      m_created(QDate::currentDate().toString()),
      m_lastMofidied(QDate::currentDate().toString()),
      m_datasetName()
{

}

GeneSelection::GeneSelection(const GeneSelection& other)
    : m_id(other.m_id),
      m_name(other.m_name),
      m_userId(other.m_userId),
      m_datasetId(other.m_datasetId),
      m_selectedItems(other.m_selectedItems),
      m_type(other.m_type),
      m_status(other.m_status),
      m_oboFroundryTerms(other.m_oboFroundryTerms),
      m_comment(other.m_comment),
      m_enabled(other.m_enabled),
      m_created(other.m_created),
      m_lastMofidied(other.m_lastMofidied),
      m_datasetName(other.m_datasetName)
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
    m_selectedItems = other.m_selectedItems;
    m_type = other.m_type;
    m_status = other.m_status;
    m_oboFroundryTerms = other.m_oboFroundryTerms;
    m_comment = other.m_comment;
    m_enabled = other.m_enabled;
    m_created = other.m_created;
    m_lastMofidied = other.m_lastMofidied;
    m_datasetName = other.m_datasetName;
    return (*this);
}

bool GeneSelection::operator==(const GeneSelection& other) const
{
    return(
                m_id == other.m_id &&
                m_name == other.m_name &&
                m_userId == other.m_userId &&
                m_datasetId == other.m_datasetId &&
                m_selectedItems == other.m_selectedItems &&
                m_type == other.m_type &&
                m_status == other.m_status &&
                m_oboFroundryTerms == other.m_oboFroundryTerms &&
                m_comment == other.m_comment &&
                m_enabled == other.m_enabled &&
                m_created == other.m_created &&
                m_lastMofidied == other.m_lastMofidied &&
                m_datasetName == other.m_datasetName
        );
}

const QString GeneSelection::id() const
{
    return m_id;
}

const QString GeneSelection::name() const
{
    return m_name;
}

const QString GeneSelection::userId() const
{
    return m_userId;
}

const QString GeneSelection::datasetId() const
{
    return m_datasetId;
}

const GeneSelection::selectedItemsList GeneSelection::selectedItems() const
{
    return m_selectedItems;
}

const QString GeneSelection::type() const
{
    return m_type;
}

const QString GeneSelection::status() const
{
    return m_status;
}

const QVector<QString> GeneSelection::oboFoundryTerms() const
{
    return m_oboFroundryTerms;
}

const QString GeneSelection::comment() const
{
    return m_comment;
}

bool GeneSelection::enabled() const
{
    return m_enabled;
}

const QString GeneSelection::created() const
{
    return m_created;
}

const QString GeneSelection::lastModified() const
{
    return m_lastMofidied;
}

const QString GeneSelection::datasetName() const
{
    return m_datasetName;
}

void GeneSelection::id(const QString& id)
{
    m_id = id;
}

void GeneSelection::name(const QString& name)
{
    m_name = name;
}

void GeneSelection::userId(const QString& userId)
{
    m_userId = userId;
}

void GeneSelection::datasetId(const QString& datasetId)
{
    m_datasetId = datasetId;
}

void GeneSelection::selectedItems(const GeneSelection::selectedItemsList& selectedItems)
{
    m_selectedItems = selectedItems;
}

void GeneSelection::type(const QString& type)
{
    m_type = type;
}

void GeneSelection::status(const QString& status)
{
    m_status = status;
}

void GeneSelection::oboFoundryTerms(const QVector<QString> &oboFoundryTerms)
{
    m_oboFroundryTerms = oboFoundryTerms;
}

void GeneSelection::comment(const QString& comment)
{
    m_comment = comment;
}

void GeneSelection::enabled(const bool enabled)
{
    m_enabled = enabled;
}

void GeneSelection::created(const QString& created)
{
    m_created = created;
}

void GeneSelection::lastModified(const QString& lastModified)
{
    m_lastMofidied = lastModified;
}

void GeneSelection::datasetName(const QString& datasetName)
{
    m_datasetName = datasetName;
}
