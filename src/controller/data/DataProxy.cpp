/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QDebug>
#include "utils/DebugHelper.h"

#include "DataProxy.h"
#include <QJsonDocument>
#include "utils/config/Configuration.h"
#include "controller/network/NetworkManager.h"
#include "controller/network/NetworkCommand.h"
#include "controller/network/NetworkReply.h"
#include "controller/network/RESTCommandFactory.h"
#include "controller/error/NetworkError.h"
#include "controller/error/ServerError.h"
#include "controller/data/DataStore.h"

// parse objects
#include "model/ObjectParser.h"
#include "model/dto/ChipDTO.h"
#include "model/dto/DatasetDTO.h"
#include "model/dto/GeneDTO.h"
#include "model/dto/FeatureDTO.h"
#include "model/dto/HitCountDTO.h"
#include "model/dto/UserDTO.h"
#include "model/dto/ErrorDTO.h"


//TODO this component will eventualy replace the hash maps in memory
// serialized/encrypted data on the HD filtered by user, dataProxy will check if data
// is present in the cache before making the REST call, although
// expered or pudated data will be re-downloaded


DataProxy::DataProxy() : m_user(nullptr)
{    
    init();
}

DataProxy::~DataProxy()
{

}

void DataProxy::init()
{
    //initialize data containers

}

void DataProxy::finalize()
{
    
    //NOTE I do not think there is a need to iterate all the containers to free memory
    // but it is good to do it since we will probably replace the containers at some point
    
/*    //clearning user container
    m_user.clear();
    //cleaning datasets map
    for(auto record : m_datasetMap.toStdMap())
    {
        record.second.clear();
    }
    m_datasetMap.clear();
    //cleaning dataset list (just in case)
    foreach(DataProxy::DatasetPtr record, (*m_datasetListPtr))
    {
        record.clear();
    }
    m_datasetListPtr.clear();
    
    //cleaning geneMap
    for(auto record : m_geneMap.toStdMap())
    {
        for(auto record2 : record.second->toStdMap())
        {
            record2.second.clear();
        }
        record.second.clear();
    }
    m_geneMap.clear();
    
    //clearning gene list map
    for(auto record : m_geneListMap.toStdMap())
    {
        foreach(DataProxy::GenePtr gene, (*record.second))
        {
            gene.clear();
        }
        record.second.clear();
    }
    m_geneListMap.clear();
    
    //clearning chips map
    for(auto record : m_chipMap.toStdMap())
    {
        record.second.clear();
    }
    m_chipMap.clear();
    
    //cleaning feature maps
    for(auto record : m_featureMap.toStdMap())
    {
        for(auto record2 : record.second->toStdMap())
        {
            record2.second.clear();
        }
        record.second.clear();
    }
    m_featureMap.clear();
    
    //clearning feature list map
    for(auto record : m_featureListMap.toStdMap())
    {
        foreach(DataProxy::FeaturePtr feature, (*record.second))
        {
            feature.clear();
        }
        record.second.clear();
    }
    m_featureListMap.clear();
    
    //clearning gene feature list map
    for(auto record : m_geneFeatureListMap.toStdMap())
    {
        for(auto record2 : record.second->toStdMap())
        {
            foreach(DataProxy::FeaturePtr feature, (*record2.second))
            {
                feature.clear();
            }
            record2.second.clear();
        }
        record.second.clear();
    }
    m_geneFeatureListMap.clear();
    
    //cleaning hitcount maps
    for(auto record : m_hitCountMap.toStdMap())
    {
        record.second.clear();
    }
    m_hitCountMap.clear();*/
    
    //cleaning download pool
    for(auto record : m_download_pool.values().toStdList())
    {
        record.clear();
    }
    m_download_pool.clear();
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

void DataProxy::slotNetworkReply(QVariant code, QVariant data)
{
    
    // get reference to network reply from sender object
    NetworkReply* reply = reinterpret_cast<NetworkReply*>(sender());

    // something went wrong (prob no connection)
    if(!reply)
    {
        //TODO download manager will kill replies that are timed out and/or deleted (NULL)
        qDebug() << "[DataProxy] : Error, a Network reply got lost, probably no connection";
        return;
    }

    const unsigned key = qvariant_cast<unsigned>(reply->property("key"));
    qDebug() << "[DataProxy] : restoring manager hash key = " << key;
    Q_ASSERT(m_download_pool.contains(key));
    QPointer<async::DownloadManager> manager = m_download_pool[key];
    Q_ASSERT_X(manager.data(), "DataProxy", "Error, downloadmanager is empty!");

    // detect network errors
    if (reply->hasErrors())
    {
        Error *error = parseErrors(reply);
        manager->addError(error);
    }
    else
    {
        const int returnCode = qvariant_cast<int>(code);
        if (returnCode == NetworkReply::CodeError)
        {
            Error* error = new Error("Data Error", "There was an error downloading data", this);
            manager->addError(error);
        }
        else if(returnCode == NetworkReply::CodeAbort)
        {
            //nothing for now
        }
        else
        {
            // convert data
            Q_ASSERT_X(data.canConvert(QVariant::Map), "DataProxy", "Network transport data must be of map type!");
            QVariantMap parameters = data.toMap();
            bool ok = parseData(reply, parameters);
            if(!ok)
            {
                //no data has been modified...nothing to do here..
            }
        }
    }
    
    //reply has been processed, lets remove from the queue
    manager->delItem(reply);
    reply->deleteLater();
    
    // was it the last reply?
    if(manager->countItems() == 0)
    {
        manager->finish();
        m_download_pool.remove(key);
        manager.clear();
    }
    
}

Error* DataProxy::parseErrors(NetworkReply* reply)
{
    const NetworkReply::ErrorList &errors = reply->errors();
    
    Error *error = 0;
    
    if(errors.count() > 1)
    {
        QString errortext;
        
        foreach(Error *e, errors)
        {
            qDebug() << "[DataProxy] Network Reply Error " << e->name() << " : " << e->description();
            errortext += (e->name() + " : " + e->description()) + "\n";
        }
        
        //NOTE need to emit a standard Error that packs all the errors descriptions
        error = new Error("Multiple Data Error", errortext, this);
        return error;
    }
    else
    {
        QJsonDocument doc = reply->getJSON();
        QVariant var = doc.toVariant();
        
        ErrorDTO dto;
        ObjectParser::parseObject(var, &dto);
        
        error = new ServerError(dto.errorName(), dto.errorDescription());
        
    }
    
    return error;
}

bool DataProxy::parseData(NetworkReply* reply, const QVariantMap& parameters)
{   
    // mark data proxy as dirty if something is changed and emit signal if so
    bool dirty = false;

    // data type
    Q_ASSERT_X(parameters.contains(Globals::PARAM_TYPE), "DataProxy", "Data type must be defined!");
    DataType type = static_cast<DataType>(qvariant_cast<int>(parameters.value(Globals::PARAM_TYPE)));

    switch (type)
    {
    // dataset
    case DatasetDataType:
    {
        QJsonDocument doc = reply->getJSON();
        // intermediary parse object
        DatasetDTO dto;
        // ensure even single items are encapsulated in a variant list
        QVariant root = doc.toVariant();
        QVariantList list = root.canConvert(QVariant::List) ? root.toList() : (QVariantList() += root);
        foreach(QVariant var, list)
        {
            ObjectParser::parseObject(var, &dto);
            //TODO should remove previous dataset pointer if it exists
            DatasetPtr dataset = DatasetPtr(new Dataset(dto.dataset()));
            m_datasetMap.insert(dto.id(),dataset);
            m_datasetList.append(dataset);
            dirty = true;
        }
        break;
    }
        // gene
    case GeneDataType:
    {
        QJsonDocument doc = reply->getJSON();
        // gene list by dataset
        Q_ASSERT_X(parameters.contains(Globals::PARAM_DATASET), "DataProxy", "GeneData must be include dataset parameter!");
        const QString datasetId = qvariant_cast<QString>(parameters.value(Globals::PARAM_DATASET));

        // intermediary parse object and end object map
        GeneDTO dto;
        GeneList geneListByDatasetId = getGeneList(datasetId);
        GeneMap geneMapByDatasetId = getGeneMap(datasetId);

        // ensure even single items are encapsulated in a variant list
        QVariant root = doc.toVariant();
        QVariantList list = root.canConvert(QVariant::List) ? root.toList() : (QVariantList() += root);
        foreach(QVariant var, list)
        {
            ObjectParser::parseObject(var, &dto);
            GenePtr gene = GenePtr(new Gene(dto.gene()));
            geneListByDatasetId.append(gene);
            geneMapByDatasetId.insert(gene->name(), gene);
            dirty = true;
        }
        break;
    }
        // chip
    case ChipDataType:
    {
        QJsonDocument doc = reply->getJSON();
        // intermediary parse object
        ChipDTO dto;
        // ensure even single items are encapsulated in a variant list
        QVariant root = doc.toVariant();
        QVariantList list = root.canConvert(QVariant::List) ? root.toList() : (QVariantList() += root);
        foreach(QVariant var, list)
        {
            ObjectParser::parseObject(var, &dto);
            //TODO should remove previous chip pointer if it exists
            m_chipMap.insert(dto.id(), ChipPtr(new Chip(dto.chip())));
            dirty = true;
        }
        break;
    }
        // feature
    case FeatureDataType:
    {
        QJsonDocument doc = reply->getJSON();
        // feature list by dataset
        Q_ASSERT_X(parameters.contains(Globals::PARAM_DATASET), "DataProxy", "FeatureData must be include dataset parameter!");
        const QString datasetId = qvariant_cast<QString>(parameters.value(Globals::PARAM_DATASET));

        // intermediary parse object and end object map
        FeatureDTO dto;
        FeatureList featureListByDatasetId = getFeatureList(datasetId);
        FeatureMap featureMapByDatasetId = getFeatureMap(datasetId);

        // ensure even single items are encapsulated in a variant list
        QVariant root = doc.toVariant();
        QVariantList list = root.canConvert(QVariant::List) ? root.toList() : (QVariantList() += root);
        foreach(QVariant var, list)
        {
            ObjectParser::parseObject(var, &dto);

            FeaturePtr feature = FeaturePtr(new FeatureExtended(dto.feature()));
            FeatureList featureListByGeneIdAndDatasetId = getGeneFeatureList(datasetId, dto.gene());
            //TODO should remove previous feature pointer if it exists
            featureMapByDatasetId.insert(feature->id(), feature);
            featureListByDatasetId.append(feature);
            featureListByGeneIdAndDatasetId.append(feature);
            dirty = true;
        }
        break;
    }
        // hit count
    case HitCountDataType:
    {
        QJsonDocument doc = reply->getJSON();
        // feature list by dataset
        Q_ASSERT_X(parameters.contains(Globals::PARAM_DATASET), "DataProxy", "HitCountData must be include dataset parameter!");
        const QString datasetId = qvariant_cast<QString>(parameters.value(Globals::PARAM_DATASET));

        // intermediary parse object
        HitCountDTO dto;

        // ensure even single items are encapsulated in a variant list
        QVariant root = doc.toVariant();
        QVariantList list = root.canConvert(QVariant::List) ? root.toList() : (QVariantList() += root);
        foreach(QVariant var, list)
        {
            ObjectParser::parseObject(var, &dto);
            //TODO should remove previous hitcout pointer if it exits
            m_hitCountMap.insert(datasetId, HitCountPtr(new HitCount(dto.hitCount())));
            dirty = true;
        }
        break;
    } // user
    case UserType:
    {
        QJsonDocument doc = reply->getJSON();
        // intermediary parse object
        UserDTO dto;

        // ensure even single items are encapsulated in a variant list
        QVariant root = doc.toVariant();
        QVariantList list = root.canConvert(QVariant::List) ? root.toList() : (QVariantList() += root);
        foreach(QVariant var, list)
        {
            ObjectParser::parseObject(var, &dto);
            //TODO should remove previous user pointer if it exits
            m_user = UserPtr(new User(dto.user()));
            dirty = true;
        }
        break;
    } // cell tissue figure
    case TissueDataType:
    {
        Q_ASSERT_X(parameters.contains(Globals::PARAM_FILE), "DataProxy", "BlueTissueData must include file parameter!");
        const QString fileid = qvariant_cast<QString>(parameters.value(Globals::PARAM_FILE));
        // keep track of file pointer
        QScopedPointer<QIODevice> device;
        device.reset(DataStore::getInstance()->accessResource(fileid, DataStore::Temporary | DataStore::Persistent | DataStore::Secure));
        // store data in file
        const bool dataOpen = device->open(QIODevice::WriteOnly);
        if (dataOpen)
        {
            qDebug() << QString("[DataProxy] Unable to open image fileid: %1").arg(fileid);
        }
        const qint64 dataWrite = device->write(reply->getRaw());
        if (dataWrite <= 0)
        {
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

const DataProxy::GeneMap& DataProxy::getGeneMap(const QString& datasetId)
{
    GeneMapMap::iterator it = m_geneMap.find(datasetId), end = m_geneMap.end();
    if (it == end)
    {
        GeneMap genemap;
        genemap.insert(QString(),new GenePtr());
        it = m_geneMap.insert(datasetId, genemap);
    }
    return it.value();
}

const DataProxy::GeneList& DataProxy::getGeneList(const QString& datasetId)
{
    GeneListMap::iterator it = m_geneListMap.find(datasetId), end = m_geneListMap.end();
    if (it == end)
    {
        it = m_geneListMap.insert(datasetId, GeneList());
    }
    return it.value();
}

const DataProxy::FeatureList& DataProxy::getFeatureList(const QString& datasetId)
{
    FeatureListMap::iterator it = m_featureListMap.find(datasetId), end = m_featureListMap.end();
    if (it == end)
    {
        it = m_featureListMap.insert(datasetId, FeatureList());
    }
    return it.value();
}

const DataProxy::FeatureMap& DataProxy::getFeatureMap(const QString& datasetId)
{
    FeatureMapMap::iterator it = m_featureMap.find(datasetId), end = m_featureMap.end();
    if (it == end)
    {
        it = m_featureMap.insert(datasetId, FeatureMap());
    }
    return it.value();
}

DataProxy::GeneRef DataProxy::getGeneRef(const QString& datasetId, const QString& geneName)
{
    GeneMapMap::iterator it = m_geneMap.find(datasetId), end = m_geneMap.end();
    if (it == end) //check if dataset key no present at all, create
    {
        GeneMap genemap = GeneMap();
        genemap.insert(geneName, GenePtr(new Gene()));
        it = m_geneMap.insert(datasetId, genemap);
    }
    
    GeneMap::iterator it2 = it.value().find(geneName), end2 = it.value().end();
    if (it2 == end2) //check if gene id key no present at all
    {
        it2 = it.value().insert(geneName, GenePtr(new Gene()));
    }

    return it2.value().get();
}

const DataProxy::FeatureList& DataProxy::getGeneFeatureList(const QString& datasetId, const QString &geneName)
{
    FeatureListGeneMap::iterator it = m_geneFeatureListMap.find(datasetId), end = m_geneFeatureListMap.end();
    if (it == end) //check if dataset key no present at all, create
    {
        FeatureListMap featuremap = FeatureListMap();
        featuremap.insert(geneName, FeatureList());
        it = m_geneFeatureListMap.insert(datasetId, featuremap);
    }
    FeatureListMap::iterator it2 = it.value().find(geneName), end2 = it.value().end();
    if (it2 == end2) //check if gene key no present at all
    {
        it2 = it.value().insert(geneName, FeatureList());
    }

    return it2.value();
}

DataProxy::FeatureRef DataProxy::getFeatureRef(const QString& datasetId, const QString &featureId)
{
    FeatureMapMap::iterator it = m_featureMap.find(datasetId), end = m_featureMap.end();
    if (it == end) //check if dataset key no present at all, create
    {
        FeatureMap featuremap = FeatureMap();
        featuremap.insert(featureId, FeaturePtr(new FeatureExtended()));
        it = m_featureMap.insert(datasetId, featuremap);
    }
    FeatureMap::iterator it2 = it.value().find(featureId), end2 = it.value().end();
    if (it2 == end2) //check if feature id key no present at all
    {
        it2 = it.value().insert(featureId, FeaturePtr(new FeatureExtended()));
    }

    return it2.value().get();
}

const DataProxy::DatasetListRef& DataProxy::getDatasetListRef() const
{
    return getContainer<DataProxy::DatasetListRef,
            DataProxy::DatasetList,DataProxy::DatasetPtr>(m_datasetList);
}

DataProxy::DatasetRef DataProxy::getDatasetRefById(const QString& datasetId)
{
    //TODO should check it exits
    return m_datasetMap.value(datasetId).get();
}

DataProxy::UserRef DataProxy::getUserRef()
{
    return m_user.get();
}

DataProxy::ChipRef DataProxy::getChipRef(const QString& chipId)
{
    ChipMap::iterator it = m_chipMap.find(chipId), end = m_chipMap.end();
    if (it == end)
    {
        it = m_chipMap.insert(chipId, ChipPtr(new Chip()));
    }
    return it.value().get();
}

DataProxy::HitCountRef DataProxy::getHitCountRef(const QString& datasetId)
{
    HitCountMap::iterator it = m_hitCountMap.find(datasetId), end = m_hitCountMap.end();
    if (it == end)
    {
        it = m_hitCountMap.insert(datasetId, HitCountPtr(new HitCount()));
    }
    return it.value().get();
}

QIODevice *DataProxy::getFigure(const QString& figureId)
{
    DataStore* dataStore = DataStore::getInstance();
    return dataStore->accessResource(figureId, ResourceStore::Temporary | ResourceStore::Persistent | ResourceStore::Secure);
}

bool DataProxy::hasDatasets() const
{
    return !m_datasetMap.isEmpty();
}

bool DataProxy::hasDataset(const QString& datasetId) const
{
    return m_datasetMap.contains(datasetId);
}

async::DataRequest* DataProxy::loadDatasets()
{
    if((bool)hasDatasets())
    {
        QPointer<async::DataRequest> request = QPointer<async::DataRequest>(new async::DataRequest());
        request->return_code(async::DataRequest::CodePresent);
        return(request.data());
    }
    
    NetworkCommand* cmd = RESTCommandFactory::getDatasets();
    NetworkManager* nm = NetworkManager::getInstance();

    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(DatasetDataType)));

    NetworkReply* reply = nm->httpRequest(cmd, QVariant(parameters));
    
    //delete the command
    cmd->deleteLater();
    
    return createRequest(reply);
}

async::DataRequest* DataProxy::loadDatasetByDatasetId(const QString& datasetId)
{   
    if((bool)hasDataset(datasetId))
    {
        QPointer<async::DataRequest> request = QPointer<async::DataRequest>(new async::DataRequest());
        request->return_code(async::DataRequest::CodePresent);
        return(request.data());
    }

    NetworkCommand* cmd = RESTCommandFactory::getDatasetByDatasetId(datasetId);
    NetworkManager* nm = NetworkManager::getInstance();

    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(DatasetDataType)));

    NetworkReply* reply = nm->httpRequest(cmd, QVariant(parameters));
    
    //delete the command
    cmd->deleteLater();
    
    return createRequest(reply);
}

async::DataRequest* DataProxy::updateDataset(const Dataset& dataset)
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
    
    return createRequest(reply);
}

bool DataProxy::hasGene(const QString& datasetId) const
{
    return m_geneListMap.contains(datasetId);
}

async::DataRequest* DataProxy::loadGenesByDatasetId(const QString& datasetId)
{
    if((bool)hasGene(datasetId))
    {
        QPointer<async::DataRequest> request = QPointer<async::DataRequest>(new async::DataRequest());
        request->return_code(async::DataRequest::CodePresent);
        return(request.data());
    }

    NetworkCommand* cmd = RESTCommandFactory::getGenesByDatasetId(datasetId);
    NetworkManager* nm = NetworkManager::getInstance();

    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(GeneDataType)));
    parameters.insert(Globals::PARAM_DATASET, QVariant(datasetId));

    NetworkReply* reply = nm->httpRequest(cmd, QVariant(parameters));
    
    //delete the command
    cmd->deleteLater();
    
    return createRequest(reply);
}

bool DataProxy::hasChip(const QString& chipId) const
{
    return m_chipMap.contains(chipId);
}

async::DataRequest* DataProxy::loadChipById(const QString& chipId)
{   
    if((bool)hasChip(chipId))
    {
        QPointer<async::DataRequest> request = QPointer<async::DataRequest>(new async::DataRequest());
        request->return_code(async::DataRequest::CodePresent);
        return(request.data());
    }

    NetworkCommand* cmd = RESTCommandFactory::getChipByChipId(chipId);
    NetworkManager* nm = NetworkManager::getInstance();

    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(ChipDataType)));

    NetworkReply* reply = nm->httpRequest(cmd, QVariant(parameters));
    
    //delete the command
    cmd->deleteLater();
    
    return createRequest(reply);
}

bool DataProxy::hasFeature(const QString& datasetId) const
{
    return m_featureListMap.contains(datasetId);
}

async::DataRequest* DataProxy::loadFeatureByDatasetId(const QString& datasetId)
{   
    if((bool)hasFeature(datasetId))
    {
        QPointer<async::DataRequest> request = QPointer<async::DataRequest>(new async::DataRequest());
        request->return_code(async::DataRequest::CodePresent);
        return(request.data());
    }

    NetworkCommand* cmd = RESTCommandFactory::getFeatureByDatasetId(datasetId);
    NetworkManager* nm = NetworkManager::getInstance();

    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(FeatureDataType)));
    parameters.insert(Globals::PARAM_DATASET, QVariant(datasetId));

    NetworkReply* reply = nm->httpRequest(cmd, QVariant(parameters));
    
    //delete the command
    cmd->deleteLater();
    
    return createRequest(reply);
}

bool DataProxy::hasFeature(const QString& datasetId, const QString& gene) const
{
    const FeatureListGeneMap::const_iterator it = m_geneFeatureListMap.find(datasetId);
    return (it != m_geneFeatureListMap.end() ? it.value().contains(gene) : false);
}

async::DataRequest* DataProxy::loadFeatureByDatasetIdAndGene(const QString& datasetId, const QString& gene)
{   
    if((bool)hasFeature(datasetId,gene))
    {
        QPointer<async::DataRequest> request = QPointer<async::DataRequest>(new async::DataRequest());
        request->return_code(async::DataRequest::CodePresent);
        return(request.data());
    }
    
    NetworkCommand* cmd = RESTCommandFactory::getFeatureByDatasetIdAndGene(datasetId, gene);
    NetworkManager* nm = NetworkManager::getInstance();

    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(FeatureDataType)));
    parameters.insert(Globals::PARAM_DATASET, QVariant(datasetId));
    parameters.insert(Globals::PARAM_GENE, QVariant(gene));

    NetworkReply* reply = nm->httpRequest(cmd, QVariant(parameters));
    
    //delete the command
    cmd->deleteLater();
    
    return createRequest(reply);
}

bool DataProxy::hasHitCount(const QString& datasetId) const
{
    return m_hitCountMap.contains(datasetId);
}

async::DataRequest* DataProxy::loadHitCountByDatasetId(const QString& datasetId)
{  
    if((bool)hasHitCount(datasetId))
    {
        QPointer<async::DataRequest> request = QPointer<async::DataRequest>(new async::DataRequest());
        request->return_code(async::DataRequest::CodePresent);
        return(request.data());
    }
    
    NetworkCommand* cmd = RESTCommandFactory::getHitCountByDatasetId(datasetId);
    NetworkManager* nm = NetworkManager::getInstance();

    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(HitCountDataType)));
    parameters.insert(Globals::PARAM_DATASET, QVariant(datasetId));

    NetworkReply* reply = nm->httpRequest(cmd, QVariant(parameters));
    
    //delete the command
    cmd->deleteLater();
    
    return createRequest(reply);
}

async::DataRequest* DataProxy::loadUser()
{
    NetworkCommand* cmd = RESTCommandFactory::getUser();
    NetworkManager* nm = NetworkManager::getInstance();

    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(UserType)));

    NetworkReply* reply = nm->httpRequest(cmd, QVariant(parameters));
    
    //delete the command
    cmd->deleteLater();
    
    return createRequest(reply);
}

bool DataProxy::hasCellTissue(const QString& name) const
{  
    DataStore* dataStore = DataStore::getInstance();
    return dataStore->hasResource(name);
}

async::DataRequest* DataProxy::loadCellTissueByName(const QString& name)
{
    if((bool)hasCellTissue(name))
    {
        QPointer<async::DataRequest> request = QPointer<async::DataRequest>(new async::DataRequest());
        request->return_code(async::DataRequest::CodePresent);
        return(request.data());
    }
    
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

async::DataRequest* DataProxy::loadDatasetContent(DataProxy::DatasetRef dataset)
{
    Q_ASSERT_X(dataset, "DataProxy", "Error dataset is empty!!");
    NetworkManager* nm = NetworkManager::getInstance();
    QList<NetworkReply*> replies;
    const QString& datasetId = dataset->id();

    //TODO ....duplicated code...refactor this

    if(!(bool)hasCellTissue(dataset->figureBlue()))
    {
        NetworkCommand* cmd = RESTCommandFactory::getCellTissueFigureByName(dataset->figureBlue());
        QVariantMap parameters;
        parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(TissueDataType)));
        parameters.insert(Globals::PARAM_FILE, QVariant(dataset->figureBlue()));
        NetworkReply* reply = nm->httpRequest(cmd, QVariant(parameters));
        replies.append(reply);
        //delete the command
        cmd->deleteLater();
    }

    Q_ASSERT_X(m_user, "DataProxy", "No user is selected");

    if(m_user->role() == Globals::ROLE_CM && !(bool)hasCellTissue(dataset->figureRed()))
    {
        NetworkCommand* cmd = RESTCommandFactory::getCellTissueFigureByName(dataset->figureRed());
        QVariantMap parameters;
        parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(TissueDataType)));
        parameters.insert(Globals::PARAM_FILE, QVariant(dataset->figureRed()));
        NetworkReply* reply = nm->httpRequest(cmd, QVariant(parameters));
        replies.append(reply);
        //delete the command
        cmd->deleteLater();
    }
    
    if(!(bool)hasHitCount(datasetId))
    {
        NetworkCommand* cmd = RESTCommandFactory::getHitCountByDatasetId(datasetId);
        QVariantMap parameters;
        parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(HitCountDataType)));
        parameters.insert(Globals::PARAM_DATASET, QVariant(datasetId));
        NetworkReply* reply = nm->httpRequest(cmd, QVariant(parameters));
        replies.append(reply);
        //delete the cmd
        delete cmd;
    }
    
    if(!(bool)hasFeature(datasetId))
    {
        NetworkCommand* cmd = RESTCommandFactory::getFeatureByDatasetId(datasetId);
        QVariantMap parameters;
        parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(FeatureDataType)));
        parameters.insert(Globals::PARAM_DATASET, QVariant(datasetId));
        NetworkReply* reply = nm->httpRequest(cmd, QVariant(parameters));
        replies.append(reply);
        //delete the command
        cmd->deleteLater();
    }
    
    if(!(bool)hasChip(dataset->chipId()))
    {
        NetworkCommand* cmd = RESTCommandFactory::getChipByChipId(dataset->chipId());
        QVariantMap parameters;
        parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(ChipDataType)));
        NetworkReply* reply = nm->httpRequest(cmd, QVariant(parameters));
        replies.append(reply);
        //delete the command
        cmd->deleteLater();
    }
    
    if(!(bool)hasGene(datasetId))
    {
        NetworkCommand* cmd = RESTCommandFactory::getGenesByDatasetId(datasetId);
        QVariantMap parameters;
        parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(GeneDataType)));
        parameters.insert(Globals::PARAM_DATASET, QVariant(datasetId));
        NetworkReply* reply = nm->httpRequest(cmd, QVariant(parameters));
        replies.append(reply);
        //delete the command
        cmd->deleteLater();
    }
    
    if(replies.isEmpty())
    {
        
        QPointer<async::DataRequest> request = QPointer<async::DataRequest>(new async::DataRequest());
        request->return_code(async::DataRequest::CodePresent);
        return(request.data());
    }
    
    return createRequest(replies);
}

async::DataRequest* DataProxy::createRequest(const QList<NetworkReply*> &replies)  //make list so I can send multiple replies
{
    //create key and add it to request
    QPointer<async::DataRequest> request = QPointer<async::DataRequest>(new async::DataRequest(this));
    QPointer<async::DownloadManager> manager = QPointer<async::DownloadManager>(new async::DownloadManager(request,this));
    
    Q_ASSERT_X(request.data(), "DataProxy", "Error creating DataRequest object!");
    Q_ASSERT_X(manager.data(), "DataProxy", "Error creating DownloadManager object!");
    
    unsigned key = qHash(manager.data());
    qDebug() << "[DataProxy] : storing manager hash key = " << key;
    
    foreach(NetworkReply *reply, replies)
    {
        if(reply == 0)
        {
            qDebug() << "[DataProxy] : Error, the NetworkReply is null, there must have been a network error";

        }
        else
        {
            reply->setProperty("key",qVariantFromValue<unsigned>(key));
            //connect reply to signal
            connect(reply, SIGNAL(signalFinished(QVariant, QVariant)), this, SLOT(slotNetworkReply(QVariant, QVariant)));
            //add reply to downloadmanager
            manager->addItem(reply);
        }
    }
    
    if(manager->countItems() != replies.count()) //NOTE if any of the replies was wrong we return error (could be worth to continue)
    {
        manager.clear(); //this deletes request
        QPointer<async::DataRequest> request = QPointer<async::DataRequest>(new async::DataRequest());
        request->return_code(async::DataRequest::CodeError);
        return(request.data());
    }

    //add downloadmanager to download pool
    m_download_pool.insert(key,manager);
    
    return request;
}

async::DataRequest* DataProxy::createRequest(NetworkReply *reply)  //make list so I can send multiple replies
{
    if(reply == 0)
    {
        qDebug() << "[DataPRoxy] : Error, the NetworkReply is null, therefore there must have been a network error";
        QPointer<async::DataRequest> request = QPointer<async::DataRequest>(new async::DataRequest());
        request->return_code(async::DataRequest::CodeError);
        return(request.data());
    }
    //create key and add it to request
    QPointer<async::DataRequest> request = QPointer<async::DataRequest>(new async::DataRequest(this));
    QPointer<async::DownloadManager> manager = QPointer<async::DownloadManager>(new async::DownloadManager(request,this));
    
    Q_ASSERT_X(request.data(), "DataProxy", "Error creating DataRequest object!");
    Q_ASSERT_X(manager.data(), "DataProxy", "Error creating DownloadManager object!");
    
    unsigned key = qHash(manager.data());
    qDebug() << "[DataProxy] : storing manager hash key = " << key;
    reply->setProperty("key",qVariantFromValue<unsigned>(key));
    
    //connect reply to signal
    connect(reply, SIGNAL(signalFinished(QVariant, QVariant)), this, SLOT(slotNetworkReply(QVariant, QVariant)));
    
    //add reply to downloadmanager
    manager->addItem(reply);
    
    //add downloadmanager to download pool
    m_download_pool.insert(key,manager);
    
    return request;
}
