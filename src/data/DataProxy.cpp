/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "DataProxy.h"

#include <QDebug>
#include "utils/DebugHelper.h"
#include <QEventLoop>
#include <QJsonDocument>
#include <QObject>

#include "config/Configuration.h"

#include "network/NetworkManager.h"
#include "network/NetworkCommand.h"
#include "network/NetworkReply.h"
#include "network/RESTCommandFactory.h"
#include "error/NetworkError.h"
#include "data/DataStore.h"
#include "network/DownloadManager.h"

// parse objects
#include "dataModel/ObjectParser.h"
#include "dataModelDTO/ChipDTO.h"
#include "dataModelDTO/DatasetDTO.h"
#include "dataModelDTO/GeneDTO.h"
#include "dataModelDTO/FeatureDTO.h"
#include "dataModelDTO/DatasetStatisticsDTO.h"
#include "dataModelDTO/UserDTO.h"
#include "dataModelDTO/UserExperimentDTO.h"

DataProxy::DataProxy() :
    m_user(0)
{
    init();
}

DataProxy::~DataProxy()
{

}

void DataProxy::init()
{
    //initialize data containers
    m_user = UserPtr(new User());
}

void DataProxy::finalize()
{
    //every data member is a smart pointer
    m_datasetMap.clear();
    m_datasetList.clear();
    m_geneMap.clear();
    m_geneListMap.clear();
    m_chipMap.clear();
    m_featureMap.clear();
    m_featureListMap.clear();
    m_geneFeatureListMap.clear();
    m_user.clear();
    m_datasetStatisticsMap.clear();
}

void DataProxy::clean()
{
    qDebug() << "Cleaning memory cache in Dataproxy";
    // clean up containers
    finalize();
    //init containers
    init();
}

void DataProxy::cleanAll()
{
    qDebug() << "Cleaning memory cache and disk cache in Dataproxy";
    DataStore::getInstance()->clearResources();
    clean();
}

bool DataProxy::parseData(NetworkReply *reply, const QVariantMap& parameters)
{
    // mark data proxy as dirty if something is changed and emit signal if so
    bool dirty = false;
    // data type
    Q_ASSERT_X(parameters.contains(Globals::PARAM_TYPE),
               "DataProxy", "Data type must be defined!");
    //get the type of data request
    const DataType type =
            static_cast<DataType>(parameters.value(Globals::PARAM_TYPE).toInt());

    //parse data according to type
    switch (type) {
    // dataset
    case DatasetDataType: {
        const QJsonDocument doc = reply->getJSON();
        // intermediary parse object
        DatasetDTO dto;
        // ensure even single items are encapsulated in a variant list
        const QVariant root = doc.toVariant();
        const QVariantList list = root.canConvert(QVariant::List) ? root.toList() : (QVariantList() += root);
        foreach(QVariant var, list) {
            ObjectParser::parseObject(var, &dto);
            DatasetPtr dataset = DatasetPtr(new Dataset(dto.dataset()));
            m_datasetMap.insert(dto.id(), dataset);
            m_datasetList.push_back(dataset);
            dirty = true;
        }
        break;
    }
    // gene
    case GeneDataType: {
        const QJsonDocument doc = reply->getJSON();
        // gene list by dataset
        Q_ASSERT_X(parameters.contains(Globals::PARAM_DATASET),
                   "DataProxy", "GeneData must be include dataset parameter!");
        const QString datasetId =
                qvariant_cast<QString>(parameters.value(Globals::PARAM_DATASET));
        // intermediary parse object and end object map
        GeneDTO dto;
        GeneList& geneListByDatasetId = getGeneList(datasetId);
        GeneMap& geneMapByDatasetId = getGeneMap(datasetId);
        // ensure even single items are encapsulated in a variant list
        const QVariant root = doc.toVariant();
        const QVariantList list = root.canConvert(QVariant::List) ? root.toList() : (QVariantList() += root);
        foreach(QVariant var, list) {
            ObjectParser::parseObject(var, &dto);
            GenePtr gene = GenePtr(new Gene(dto.gene()));
            geneListByDatasetId.push_back(gene);
            geneMapByDatasetId.insert(gene->name(), gene);
            dirty = true;
        }
        break;
    }
    // chip
    case ChipDataType: {
        const QJsonDocument doc = reply->getJSON();
        // intermediary parse object
        ChipDTO dto;
        // ensure even single items are encapsulated in a variant list
        const QVariant root = doc.toVariant();
        const QVariantList list = root.canConvert(QVariant::List) ? root.toList() : (QVariantList() += root);
        foreach(QVariant var, list) {
            ObjectParser::parseObject(var, &dto);
            m_chipMap.insert(dto.id(), ChipPtr(new Chip(dto.chip())));
            dirty = true;
        }
        break;
    }
    // feature
    case FeatureDataType: {
        const QJsonDocument doc = reply->getJSON();
        // feature list by dataset
        Q_ASSERT_X(parameters.contains(Globals::PARAM_DATASET),
                   "DataProxy", "FeatureData must be include dataset parameter!");
        const QString datasetId =
                qvariant_cast<QString>(parameters.value(Globals::PARAM_DATASET));
        // intermediary parse object and end object map
        FeatureDTO dto;
        FeatureList& featureListByDatasetId = getFeatureList(datasetId);
        FeatureMap& featureMapByDatasetId = getFeatureMap(datasetId);
        // ensure even single items are encapsulated in a variant list
        const QVariant root = doc.toVariant();
        const QVariantList list = root.canConvert(QVariant::List) ? root.toList() : (QVariantList() += root);
        foreach(QVariant var, list) {
            ObjectParser::parseObject(var, &dto);
            FeaturePtr feature = FeaturePtr(new FeatureExtended(dto.feature()));
            FeatureList& featureListByGeneIdAndDatasetId = getGeneFeatureList(datasetId, dto.gene());
            featureMapByDatasetId.insert(feature->id(), feature);
            featureListByDatasetId.push_back(feature);
            featureListByGeneIdAndDatasetId.push_back(feature);
            dirty = true;
        }
        break;
    }
    // hit count
    case HitCountDataType: {
        QJsonDocument doc = reply->getJSON();
        // feature list by dataset
        Q_ASSERT_X(parameters.contains(Globals::PARAM_DATASET),
                   "DataProxy", "HitCountData must be include dataset parameter!");
        const QString datasetId =
                qvariant_cast<QString>(parameters.value(Globals::PARAM_DATASET));
        // intermediary parse object
        DatasetStatisticsDTO dto;
        // ensure even single items are encapsulated in a variant list
        const QVariant root = doc.toVariant();
        const QVariantList list = root.canConvert(QVariant::List) ? root.toList() : (QVariantList() += root);
        foreach(QVariant var, list) {
            ObjectParser::parseObject(var, &dto);
            m_datasetStatisticsMap.insert(datasetId,
                                          DatasetStatisticsPtr(new DatasetStatistics(dto.datasetStatistics())));
            dirty = true;
        }
        break;
    } // user
    case UserType: {
        const QJsonDocument doc = reply->getJSON();
        // intermediary parse object
        UserDTO dto;
        // ensure even single items are encapsulated in a variant list
        const QVariant root = doc.toVariant();
        const QVariantList list = root.canConvert(QVariant::List) ? root.toList() : (QVariantList() += root);
        foreach(QVariant var, list) {
            ObjectParser::parseObject(var, &dto);
            m_user = UserPtr(new User(dto.user()));
            dirty = true;
        }
        break;
    } // cell tissue figure
    case TissueDataType: {
        Q_ASSERT_X(parameters.contains(Globals::PARAM_FILE),
                   "DataProxy", "BlueTissueData must include file parameter!");
        const QString fileid = qvariant_cast<QString>(parameters.value(Globals::PARAM_FILE));
        // keep track of file pointer
        QScopedPointer<QIODevice> device;
        device.reset(DataStore::getInstance()->accessResource(fileid,
                                                              DataStore::Temporary |
                                                              DataStore::Persistent |
                                                              DataStore::Secure));
        // store data in file
        const bool dataOpen = device->open(QIODevice::WriteOnly);
        if (dataOpen) {
            qDebug() << QString("[DataProxy] Unable to open image fileid: %1").arg(fileid);
        }
        const qint64 dataWrite = device->write(reply->getRaw());
        if (dataWrite <= 0) {
            qDebug() << QString("[DataProxy] Unable to write data to fileid: %1").arg(fileid);
        }
        device->close();
        dirty = true;
        break;
    }
    default:
        qDebug() << "[DataProxy] Error: Unknown data type!";
    }

    return dirty;
}

DataProxy::GeneMap& DataProxy::getGeneMap(const QString& datasetId)
{
    GeneMapMap::iterator it = m_geneMap.find(datasetId);
    GeneMapMap::iterator end = m_geneMap.end();
    if (it == end) {
        it = m_geneMap.insert(datasetId, GeneMap());
    }
    return it.value();
}

DataProxy::GeneList& DataProxy::getGeneList(const QString& datasetId)
{
    GeneListMap::iterator it = m_geneListMap.find(datasetId);
    GeneListMap::iterator end = m_geneListMap.end();
    if (it == end) {
        it = m_geneListMap.insert(datasetId, GeneList());
    }
    return it.value();
}

DataProxy::FeatureList& DataProxy::getFeatureList(const QString& datasetId)
{
    FeatureListMap::iterator it = m_featureListMap.find(datasetId);
    FeatureListMap::iterator end = m_featureListMap.end();
    if (it == end) {
        it = m_featureListMap.insert(datasetId, FeatureList());
    }
    return it.value();
}

DataProxy::FeatureMap& DataProxy::getFeatureMap(const QString& datasetId)
{
    FeatureMapMap::iterator it = m_featureMap.find(datasetId);
    FeatureMapMap::iterator end = m_featureMap.end();
    if (it == end) {
        it = m_featureMap.insert(datasetId, FeatureMap());
    }
    return it.value();
}

DataProxy::GenePtr DataProxy::getGene(const QString& datasetId,
                                      const QString& geneName)
{
    GeneMapMap::iterator it = m_geneMap.find(datasetId);
    GeneMapMap::iterator end = m_geneMap.end();
    if (it == end) { //check if dataset key no present at all, create
        GeneMap genemap;
        genemap.insert(geneName, GenePtr(new Gene()));
        it = m_geneMap.insert(datasetId, genemap);
    }
    GeneMap::iterator it2 = it.value().find(geneName);
    GeneMap::iterator end2 = it.value().end();
    if (it2 == end2) { //check if gene id key no present at all
        it2 = it.value().insert(geneName, GenePtr(new Gene()));
    }

    return it2.value();
}

DataProxy::FeatureList& DataProxy::getGeneFeatureList(const QString& datasetId,
                                                        const QString &geneName)
{
    FeatureListGeneMap::iterator it = m_geneFeatureListMap.find(datasetId);
    FeatureListGeneMap::iterator end = m_geneFeatureListMap.end();
    if (it == end) { //check if dataset key no present at all, create
        FeatureListMap featuremap;
        featuremap.insert(geneName, FeatureList());
        it = m_geneFeatureListMap.insert(datasetId, featuremap);
    }
    FeatureListMap::iterator it2 = it.value().find(geneName);
    FeatureListMap::iterator end2 = it.value().end();
    if (it2 == end2) { //check if gene key no present at all
        it2 = it.value().insert(geneName, FeatureList());
    }
    return it2.value();
}

DataProxy::FeaturePtr DataProxy::getFeature(const QString& datasetId,
                                            const QString &featureId)
{
    FeatureMapMap::iterator it = m_featureMap.find(datasetId);
    FeatureMapMap::iterator end = m_featureMap.end();
    if (it == end) { //check if dataset key no present at all, create
        FeatureMap featuremap;;
        featuremap.insert(featureId, FeaturePtr(new FeatureExtended()));
        it = m_featureMap.insert(datasetId, featuremap);
    }
    FeatureMap::iterator it2 = it.value().find(featureId);
    FeatureMap::iterator end2 = it.value().end();
    if (it2 == end2) { //check if feature id key no present at all
        it2 = it.value().insert(featureId, FeaturePtr(new FeatureExtended()));
    }
    return it2.value();
}

const DataProxy::DatasetList &DataProxy::getDatasetList() const
{
    return m_datasetList;
}

DataProxy::DatasetPtr DataProxy::getDatasetById(const QString& datasetId) const
{
    return m_datasetMap.value(datasetId);
}

DataProxy::UserPtr DataProxy::getUser() const
{
    return m_user;
}

DataProxy::ChipPtr DataProxy::getChip(const QString& chipId)
{
    ChipMap::const_iterator it = m_chipMap.find(chipId);
    ChipMap::const_iterator end = m_chipMap.end();
    if (it == end) {
        it = m_chipMap.insert(chipId, ChipPtr(new Chip()));
    }
    return it.value();
}

DataProxy::DatasetStatisticsPtr DataProxy::getStatistics(const QString& datasetId)
{
    DatasetStatisticsMap::const_iterator it = m_datasetStatisticsMap.find(datasetId);
    DatasetStatisticsMap::const_iterator end = m_datasetStatisticsMap.end();
    if (it == end) {
        it = m_datasetStatisticsMap.insert(datasetId,
                                           DatasetStatisticsPtr(new DatasetStatistics()));
    }
    return it.value();
}

QIODevice *DataProxy::getFigure(const QString& figureId) const
{
    DataStore* dataStore = DataStore::getInstance();
    return dataStore->accessResource(figureId,
                                     ResourceStore::Temporary |
                                     ResourceStore::Persistent |
                                     ResourceStore::Secure);
}

const DataProxy::UserExperimentList& DataProxy::getSelectedObjects() const
{
    return m_selectedObjects;
}

const QString DataProxy::getSelectedDataset() const
{
    return m_selected_datasetId;
}

void DataProxy::setSelectedDataset(const QString &datasetId) const
{
    m_selected_datasetId = datasetId;
}

DataProxy::UniqueGeneSelectedList DataProxy::getUniqueGeneSelected(const qreal roof,
                                                                   const FeatureList &features)
{
    //TODO this can be optimized to do in one loop
    QMap<QString, GeneSelection> geneSelectionsMap;
    DataProxy::UniqueGeneSelectedList geneSelectionsList;
    foreach(DataProxy::FeaturePtr feature, features) {
        if (!feature->selected()) {
            continue;
        }
        const QString name = feature->gene();
        const qreal reads = feature->hits();
        const qreal normalizedReads = feature->hits() / roof;
        if (geneSelectionsMap.count( name ) == 0) {
            GeneSelection newselection(name, reads, normalizedReads);
            geneSelectionsMap.insert(feature->gene(), newselection);
        }
        else {
            const qreal currentReads = geneSelectionsMap[name].reads();
            const qreal newReads = currentReads + reads;
            geneSelectionsMap[name].reads(newReads);
            geneSelectionsMap[name].normalizedReads(newReads / roof);
        }
    }
    QMap<QString, GeneSelection>::const_iterator it = geneSelectionsMap.begin();
    QMap<QString, GeneSelection>::const_iterator end = geneSelectionsMap.end();
    for( ; it != end; ++it) {
        geneSelectionsList.append(it.value());
    }

    return geneSelectionsList;
}

bool DataProxy::hasDatasets() const
{
    return !m_datasetMap.isEmpty();
}

bool DataProxy::hasDataset(const QString& datasetId) const
{
    return m_datasetMap.contains(datasetId);
}

async::DataRequest DataProxy::loadDatasets()
{
    //check if present already
    if (hasDatasets()) {
        async::DataRequest request;
        request.return_code(async::DataRequest::CodePresent);
        return request;
    }
    //creates the request
    NetworkCommand* cmd = RESTCommandFactory::getDatasets();
    NetworkManager* nm = NetworkManager::getInstance();
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(DatasetDataType)));
    NetworkReply* reply = nm->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    //return the request
    return createRequest(reply);
}

async::DataRequest DataProxy::updateDataset(const Dataset& dataset)
{
    // intermediary dto object
    DatasetDTO dto(dataset);
    NetworkCommand* cmd = RESTCommandFactory::updateDatsetByDatasetId(dto.id());
    // add all (meta) properties of the dto as query items
    cmd->addQueryItems(&dto);
    NetworkManager* nm = NetworkManager::getInstance();
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(DatasetDataType)));
    NetworkReply* reply = nm->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    //return the request
    return createRequest(reply);
}

bool DataProxy::hasGene(const QString& datasetId) const
{
    return m_geneListMap.contains(datasetId);
}

async::DataRequest DataProxy::loadGenesByDatasetId(const QString& datasetId)
{
    //check if present already
    if (hasGene(datasetId)) {
        async::DataRequest request;
        request.return_code(async::DataRequest::CodePresent);
        return request;
    }
    //creates the request
    NetworkCommand* cmd = RESTCommandFactory::getGenesByDatasetId(datasetId);
    NetworkManager* nm = NetworkManager::getInstance();
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(GeneDataType)));
    parameters.insert(Globals::PARAM_DATASET, QVariant(datasetId));
    NetworkReply* reply = nm->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    //returns the request
    return createRequest(reply);
}

bool DataProxy::hasChip(const QString& chipId) const
{
    return m_chipMap.contains(chipId);
}

async::DataRequest DataProxy::loadChipById(const QString& chipId)
{
    //check if present already
    if (hasChip(chipId)) {
        async::DataRequest request;
        request.return_code(async::DataRequest::CodePresent);
        return request;
    }
    //creates the request
    NetworkCommand* cmd = RESTCommandFactory::getChipByChipId(chipId);
    NetworkManager* nm = NetworkManager::getInstance();
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(ChipDataType)));
    NetworkReply* reply = nm->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    //returns the request
    return createRequest(reply);
}

bool DataProxy::hasFeature(const QString& datasetId) const
{
    return m_featureListMap.contains(datasetId);
}

async::DataRequest DataProxy::loadFeatureByDatasetId(const QString& datasetId)
{
    //check if present already
    if (hasFeature(datasetId)) {
        async::DataRequest request;
        request.return_code(async::DataRequest::CodePresent);
        return request;
    }
    //creates the request
    NetworkCommand* cmd = RESTCommandFactory::getFeatureByDatasetId(datasetId);
    NetworkManager* nm = NetworkManager::getInstance();
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(FeatureDataType)));
    parameters.insert(Globals::PARAM_DATASET, QVariant(datasetId));
    NetworkReply* reply = nm->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    //returns the request
    return createRequest(reply);
}

bool DataProxy::hasFeature(const QString& datasetId, const QString& gene) const
{
    const FeatureListGeneMap::const_iterator it = m_geneFeatureListMap.find(datasetId);
    return (it != m_geneFeatureListMap.end() ? it.value().contains(gene) : false);
}

bool DataProxy::hasStatistics(const QString& datasetId) const
{
    return m_datasetStatisticsMap.contains(datasetId);
}

async::DataRequest DataProxy::loadDatasetStatisticsByDatasetId(const QString& datasetId)
{
    //check if present already
    if (hasStatistics(datasetId)) {
        async::DataRequest request;
        request.return_code(async::DataRequest::CodePresent);
        return request;
    }
    //creates the request
    NetworkCommand* cmd = RESTCommandFactory::getHitCountByDatasetId(datasetId);
    NetworkManager* nm = NetworkManager::getInstance();
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(HitCountDataType)));
    parameters.insert(Globals::PARAM_DATASET, QVariant(datasetId));
    NetworkReply* reply = nm->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    //returns the request
    return createRequest(reply);
}

async::DataRequest DataProxy::loadUser()
{
    //no need to check if present (we always reload the user)
    //creates the requet
    NetworkCommand* cmd = RESTCommandFactory::getUser();
    NetworkManager* nm = NetworkManager::getInstance();
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(UserType)));
    NetworkReply* reply = nm->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    //returns the request
    return createRequest(reply);
}

async::DataRequest DataProxy::loadSelectionObjects()
{
    //no need to check if present (we always reload the selections)
    //creates the requet
    NetworkCommand* cmd = RESTCommandFactory::getSelections();
    NetworkManager* nm = NetworkManager::getInstance();
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(UserExperimentDataType)));
    NetworkReply* reply = nm->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    //returns the request
    return createRequest(reply);
}

bool DataProxy::hasCellTissue(const QString& name) const
{
    DataStore* dataStore = DataStore::getInstance();
    return dataStore->hasResource(name);
}

async::DataRequest DataProxy::loadCellTissueByName(const QString& name)
{
    //check if present already
    if (hasCellTissue(name)) {
        async::DataRequest request;
        request.return_code(async::DataRequest::CodePresent);
        return request;
    }
    //creates the request
    NetworkCommand* cmd = RESTCommandFactory::getCellTissueFigureByName(name);
    NetworkManager* nm = NetworkManager::getInstance();
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(TissueDataType)));
    parameters.insert(Globals::PARAM_FILE, QVariant(name));
    NetworkReply* reply = nm->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    return createRequest(reply);
}

async::DataRequest DataProxy::createRequest(NetworkReply *reply)
{
    async::DataRequest request;

    if ( reply == nullptr ) {
        qDebug() << "[DataPRoxy] : Error, the NetworkReply is null,"
                    " therefore there must have been a network error";
        request.return_code(async::DataRequest::CodeError);
        return request;
    }

    QEventLoop loop; // I want to wait until this finishes
    QObject::connect(reply, SIGNAL(signalFinished(QVariant, QVariant)), &loop,SLOT(quit()));
    loop.exec();

    //TODO add slot Abort to DataRequest and connect it to the reply

    if (reply->hasErrors()) {
        const Error *error = reply->parseErrors();
        request.addError(error);
        request.return_code(async::DataRequest::CodeError);
    } else {
        const NetworkReply::ReturnCode returnCode =
                static_cast<NetworkReply::ReturnCode>(reply->return_code());
        if (returnCode == NetworkReply::CodeError) {
            Error* error = new Error("Data Error", "There was an error downloading data", nullptr);
            request.addError(error);
            request.return_code(async::DataRequest::CodeError);
        } else if (returnCode == NetworkReply::CodeAbort) {
            //nothing for now
            request.return_code(async::DataRequest::CodeAbort);
        } else {
            // convert data
            const QVariant data = reply->customData();
            Q_ASSERT_X(data.canConvert(QVariant::Map),
                       "DataProxy", "Network transport data must be of map type!");
            const QVariantMap parameters = data.toMap();
            const bool ok = parseData(reply, parameters);
            if (!ok) {
                //no data has been modified...nothing to do here..
            }
            request.return_code(async::DataRequest::CodeSuccess);
        }
    }

    //reply has been processed, lets remove it
    reply->deleteLater();
    //return request
    return request;
}
