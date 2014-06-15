#include "GeneSelection.h"

#include <QMap>

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
      m_resultFile()
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
      m_resultFile(other.m_resultFile)
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
    m_resultFile = other.m_resultFile;
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
                m_resultFile == other.m_resultFile
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

const QString GeneSelection::resultFile() const
{
    return m_resultFile;
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

void GeneSelection::resultFile(const QString& file)
{
    m_resultFile = file;
}

const GeneSelection::selectedItemsList
GeneSelection::getUniqueSelectedItems(const selectedItemsList &selectedItems)
{
    //TODO this can be optimized to do in one loop
    //TODO updgrade this to also account for normalized reads and pixel intensities
    QMap<QString, qreal> geneSelectionsMap;
    GeneSelection::selectedItemsList geneSelectionsList;
    foreach(const SelectionType &item, selectedItems) {
        geneSelectionsMap[item.name] += item.reads;
    }

    QMap<QString, qreal>::const_iterator it = geneSelectionsMap.begin();
    QMap<QString, qreal>::const_iterator end = geneSelectionsMap.end();
    for( ; it != end; ++it) {
        geneSelectionsList.append(SelectionType(it.key(), it.value()));
    }

    return geneSelectionsList;
}
