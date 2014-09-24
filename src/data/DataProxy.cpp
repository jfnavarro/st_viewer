/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "DataProxy.h"

#include <QDebug>
#include <QEventLoop>
#include <QJsonDocument>
#include <QObject>
#include <QtGlobal>

#include "config/Configuration.h"
#include "network/NetworkManager.h"
#include "network/NetworkCommand.h"
#include "network/NetworkReply.h"
#include "network/RESTCommandFactory.h"
#include "network/DownloadManager.h"
#include "error/NetworkError.h"

// parse objects
#include "data/ObjectParser.h"
#include "dataModel/ChipDTO.h"
#include "dataModel/DatasetDTO.h"
#include "dataModel/FeatureDTO.h"
#include "dataModel/ImageAlignmentDTO.h"
#include "dataModel/UserDTO.h"
#include "dataModel/GeneSelectionDTO.h"
#include "dataModel/LastModifiedDTO.h"
#include "dataModel/WrappedFileDTO.h"

static const DataStore::Options resourceFlags = DataStore::Temporary |
        DataStore::Persistent |
        DataStore::Secure;

DataProxy::DataProxy(QObject *parent) :
    QObject(parent),
    m_user(nullptr),
    m_networkManager(nullptr),
    m_authorizationManager(nullptr)
{
    //initialize data containers
    m_user = UserPtr(new User());

    m_networkManager = new NetworkManager(m_configurationManager);
    Q_ASSERT(!m_networkManager.isNull());

    m_authorizationManager =
            new AuthorizationManager(m_networkManager, m_configurationManager);
    Q_ASSERT(!m_authorizationManager.isNull());
}

DataProxy::~DataProxy()
{
    m_networkManager->deleteLater();
    m_networkManager = nullptr;

    m_authorizationManager->deleteLater();
    m_authorizationManager = nullptr;

    m_user.clear();

    clean();
}

void DataProxy::clean()
{
    qDebug() << "Cleaning memory cache in Dataproxy";
    //every data member is a smart pointer
    //TODO make totally sure data is being de-allocated
    m_datasetMap.clear();
    m_geneMap.clear();
    m_geneListMap.clear();
    m_chipMap.clear();
    m_featureMap.clear();
    m_featureListMap.clear();
    m_geneFeatureListMap.clear();
    m_imageAlignmentMap.clear();
    m_geneSelectionsMap.clear();
    m_selected_datasetId.clear();
}

void DataProxy::cleanAll()
{
    qDebug() << "Cleaning memory cache and disk cache in Dataproxy";
    m_dataStore.clearResources();
    clean();
}

QPointer<AuthorizationManager> DataProxy::getAuthorizationManager() const
{
    //DataProxy owns Authorization Manager
    //TODO it should be owned by the main class instead
    return m_authorizationManager;
}

//TODO too big function (SPLIT ASAP)
bool DataProxy::parseData(NetworkReply *reply, const QVariantMap& parameters)
{
    // mark data proxy as dirty if something is changed
    bool dirty = false;

    // data type
    Q_ASSERT(parameters.contains(Globals::PARAM_TYPE));

    //get the type of data request
    const DataType type =
            static_cast<DataType>(parameters.value(Globals::PARAM_TYPE).toInt());

    //parse data according to type
    switch (type) {
    // dataset
    case DatasetDataType: {
        const QJsonDocument doc = reply->getJSON();
        if (doc.isNull() || doc.isEmpty()) {
            return false;
        }
        //clean up cached datasets
        m_datasetMap.clear();
        // intermediary parse object
        DatasetDTO dto;
        // ensure even single items are encapsulated in a variant list
        const QVariant root = doc.toVariant();
        const QVariantList list = root.canConvert(QVariant::List) ? root.toList()
                                                                  : (QVariantList() += root);
        //parse the objects
        foreach(QVariant var, list) {
            data::parseObject(var, &dto);
            DatasetPtr dataset = DatasetPtr(new Dataset(dto.dataset()));
            m_datasetMap.insert(dataset->id(), dataset);
            dirty = true;
        }
        break;
    }
        // chip
    case ChipDataType: {
        const QJsonDocument doc = reply->getJSON();
        if (doc.isNull() || doc.isEmpty()) {
            return false;
        }
        // intermediary parse object
        ChipDTO dto;
        // should only be one item
        const QVariant root = doc.toVariant();
        //parse the data
        data::parseObject(root, &dto);
        ChipPtr chip = ChipPtr(new Chip(dto.chip()));
        m_chipMap.insert(chip->id(), chip);
        dirty = true;
        break;
    }
        // image alignment
    case ImageAlignmentDataType: {
        const QJsonDocument doc = reply->getJSON();
        if (doc.isNull() || doc.isEmpty()) {
            return false;
        }
        // intermediary parse object
        ImageAlignmentDTO dto;
        // image alignment should only contain one object
        const QVariant root = doc.toVariant();
        // parse the data
        data::parseObject(root, &dto);
        ImageAlignmentPtr imageAlignement =
                ImageAlignmentPtr(new ImageAlignment(dto.imageAlignment()));
        m_imageAlignmentMap.insert(imageAlignement->id(), imageAlignement);
        dirty = true;
        break;
    }
        // gene selection
    case GeneSelectionDataType: {
        const QJsonDocument doc = reply->getJSON();
        if (doc.isNull() || doc.isEmpty()) {
            return false;
        }
        //clean up the current cached selections
        m_geneSelectionsMap.clear();
        // intermediary parse object
        GeneSelectionDTO dto;
        // ensure even single items are encapsulated in a variant list
        const QVariant root = doc.toVariant();
        const QVariantList list = root.canConvert(QVariant::List) ? root.toList()
                                                                  : (QVariantList() += root);
        //parse the data
        foreach(QVariant var, list) {
            data::parseObject(var, &dto);
            GeneSelectionPtr selection = GeneSelectionPtr(new GeneSelection(dto.geneSelection()));
            //get the dataset name from the cached datasets
            //TODO if we allow to enter the Analysis View without having entered
            //the Datasets View this will fail. In that case, the dataset has to be retrieved
            //from the network by its ID
            const DatasetPtr dataset = getDatasetById(selection->datasetId());
            Q_ASSERT(!dataset.isNull());
            selection->datasetName(dataset->name());
            m_geneSelectionsMap.insert(selection->id(), selection);
            dirty = true;
        }
        break;
    }
        // feature
    case FeatureDataType: {
        const QJsonDocument doc = reply->getJSON();
        if (doc.isNull() || doc.isEmpty()) {
            return false;
        }

        //use file wrapper DTO to parse the JSON wraper
        WrappedFileDTO dto;
        const QVariant root = doc.toVariant();
        data::parseObject(root, &dto);

        // gene list by dataset
        Q_ASSERT_X(parameters.contains(Globals::PARAM_DATASET),
                   "DataProxy", "FeatureData must include dataset parameter!");
        const QString datasetId = parameters.value(Globals::PARAM_DATASET).toString();

        //create resource file
        DataStore::resourceDeviceType device =
                m_dataStore.createResource(datasetId, resourceFlags);

        //open file to write
        Q_ASSERT(!device->isOpen());
        const bool dataOpen = device->open(QIODevice::WriteOnly);
        if (!dataOpen) {
            qDebug() << QString("[DataProxy] Unable to open json file: %1").arg(datasetId);
            return false;
        }

        //store data in file
        const qint64 dataWrite = device->write(dto.decompressedFile());
        device->close();

        if (dataWrite <= 0) {
            qDebug() << QString("[DataProxy] Unable to write data to json file: %1").arg(datasetId);
            dirty = false;
        } else {
            dirty = parseFeaturesJSONfile(datasetId);
        }

        break;
    }
        // user
    case UserType: {
        const QJsonDocument doc = reply->getJSON();
        if (doc.isNull() || doc.isEmpty()) {
            return false;
        }
        // intermediary parse object
        UserDTO dto;
        // should only be one item
        const QVariant root = doc.toVariant();
        data::parseObject(root, &dto);
        m_user = UserPtr(new User(dto.user()));
        dirty = true;
        break;
    }
        // cell tissue figure
    case TissueDataType: {
        Q_ASSERT_X(parameters.contains(Globals::PARAM_FILE),
                   "DataProxy", "Tissue image must include file parameter!");
        const QString fileid = parameters.value(Globals::PARAM_FILE).toString();

        //create resource file
        DataStore::resourceDeviceType device =
                m_dataStore.createResource(fileid, resourceFlags);

        //open file to write
        Q_ASSERT(!device->isOpen());
        const bool dataOpen = device->open(QIODevice::WriteOnly);
        if (!dataOpen) {
            qDebug() << QString("[DataProxy] Unable to open image file: %1").arg(fileid);
            return false;
        }

        //store data in file
        const qint64 dataWrite = device->write(reply->getRaw());
        device->close();

        if (dataWrite <= 0) {
            qDebug() << QString("[DataProxy] Unable to write data to image file: %1").arg(fileid);
            dirty = false;
        }

        break;
    }
        // min supported version
    case MinVersionType: {
        const QJsonDocument doc = reply->getJSON();
        if (doc.isNull() || doc.isEmpty()) {
            return false;
        }
        // intermediary parse object
        MinVersionDTO dto;
        // should only be one item
        const QVariant root = doc.toVariant();
        data::parseObject(root, &dto);
        m_minVersion = dto.minVersionAsNumber();
        dirty = true;
        break;
    }
        // None when an update is being performed
    case None: {
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

DataProxy::GeneList &DataProxy::getGeneList(const QString& datasetId)
{
    GeneListMap::iterator it = m_geneListMap.find(datasetId);
    GeneListMap::iterator end = m_geneListMap.end();
    if (it == end) {
        it = m_geneListMap.insert(datasetId, GeneList());
    }
    return it.value();
}

DataProxy::FeatureList &DataProxy::getFeatureList(const QString& datasetId)
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


DataProxy::FeatureList &DataProxy::getGeneFeatureList(const QString& datasetId,
                                                      const QString &geneName)
{
    FeatureListGeneMap::iterator it = m_geneFeatureListMap.find(datasetId);
    FeatureListGeneMap::iterator end = m_geneFeatureListMap.end();
    //check if dataset key no present at all, create
    if (it == end) {
        FeatureListMap featuremap;
        featuremap.insert(geneName, FeatureList());
        it = m_geneFeatureListMap.insert(datasetId, featuremap);
    }

    FeatureListMap::iterator it2 = it.value().find(geneName);
    FeatureListMap::iterator end2 = it.value().end();
    //check if gene key no present at all
    if (it2 == end2) {
        it2 = it.value().insert(geneName, FeatureList());
    }

    return it2.value();
}

const DataProxy::DatasetList DataProxy::getDatasetList() const
{
    return m_datasetMap.values();
}

DataProxy::DatasetPtr DataProxy::getDatasetById(const QString& datasetId) const
{
    Q_ASSERT(m_datasetMap.contains(datasetId));
    return m_datasetMap.value(datasetId);
}

DataProxy::UserPtr DataProxy::getUser() const
{
    Q_ASSERT(!m_user.isNull());
    return m_user;
}

DataProxy::ChipPtr DataProxy::getChip(const QString& chipId)
{
    Q_ASSERT(m_chipMap.contains(chipId));
    return m_chipMap.value(chipId);
}

DataProxy::ImageAlignmentPtr DataProxy::getImageAlignment(const QString& imageAlignmentId)
{
    Q_ASSERT(m_imageAlignmentMap.contains(imageAlignmentId));
    return m_imageAlignmentMap.value(imageAlignmentId);
}

//returned resource could be NULLPTR
DataStore::resourceDeviceType DataProxy::getFigure(const QString& figureId)
{
    return m_dataStore.accessResource(figureId, resourceFlags);
}

const DataProxy::GeneSelectionList DataProxy::getGenesSelectionsList() const
{
    return m_geneSelectionsMap.values();
}

const QString DataProxy::getSelectedDataset() const
{
    //used to keep track of what dataset is currently selected
    return m_selected_datasetId;
}

void DataProxy::setSelectedDataset(const QString &datasetId) const
{
    //used to keep track of what dataset is currently selected
    m_selected_datasetId = datasetId;
}

const DataProxy::MinVersionArray DataProxy::getMinVersion() const
{
    return m_minVersion;
}

async::DataRequest DataProxy::loadDatasets()
{
    //For the moment is safer to no use the cached datasets
    //and force to download them every time

    //creates the request
    NetworkCommand *cmd = RESTCommandFactory::getDatasets(m_configurationManager);
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    //QUuid encloses its uuids in "{}"
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36));
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(DatasetDataType)));
    NetworkReply *reply = m_networkManager->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    //return the request
    return createRequest(reply);
}

async::DataRequest DataProxy::updateDataset(DatasetPtr dataset)
{
    //To avoid any type of race conditions, we enforce
    //to peform a GET of the datasets after updating one of them.
    //Currently the datasets are not cached

    // intermediary dto object
    DatasetDTO dto(*dataset);
    NetworkCommand *cmd =
            RESTCommandFactory::updateDatasetByDatasetId(m_configurationManager, dataset->id());
    //append json data
    cmd->setJsonQuery(dto.toJson());
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    //QUuid encloses its uuids in "{}"
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36));
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(None)));
    NetworkReply *reply = m_networkManager->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    //return the request
    return createRequest(reply);
}

async::DataRequest DataProxy::removeDataset(const QString& datasetId)
{
    //To avoid any type of race conditions, we enforce
    //to peform a GET of the datasets after removing one of them.
    //Currently the datasets are not cached

    NetworkCommand *cmd =
            RESTCommandFactory::removeDatasetByDatasetId(m_configurationManager, datasetId);
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    //QUuid encloses its uuids in "{}"
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36));
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(None)));
    NetworkReply *reply = m_networkManager->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    //return the request
    return createRequest(reply);
}

bool DataProxy::hasChip(const QString& chipId) const
{
    const qlonglong last_modified = getChipLastModified(chipId);
    if (last_modified == -1) {
        qDebug() << "There was an error retrieving the "
                 << "last_modified for the Chip " << chipId;
        return false;
    }

    return m_chipMap.contains(chipId)
            && m_chipMap.value(chipId)->lastModified().toLongLong() == last_modified;
}

async::DataRequest DataProxy::loadChipById(const QString& chipId)
{
    if (hasChip(chipId)) {
        return async::DataRequest(async::DataRequest::CodePresent);
    }

    //creates the request
    NetworkCommand *cmd =
            RESTCommandFactory::getChipByChipId(m_configurationManager, chipId);
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    //QUuid encloses its uuids in "{}"
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36));
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(ChipDataType)));
    NetworkReply *reply = m_networkManager->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    //returns the request
    return createRequest(reply);
}

//last modified will be added to the resource store if not present
bool DataProxy::hasFeatures(const QString& datasetId)
{
    const qlonglong last_modified = getFeaturesLastModified(datasetId);
    if (last_modified == -1) {
        qDebug() << "There was an error retrieving the "
                 << "last_modified for the Features file " << datasetId;
        return false;
    }

    return m_dataStore.hasResource(datasetId) &&
            !m_dataStore.resourceIsModified(datasetId, last_modified);
}

async::DataRequest DataProxy::loadFeatureByDatasetId(const QString& datasetId)
{
    if (hasFeatures(datasetId)) {
        async::DataRequest request(async::DataRequest::CodePresent);

        //check if they are already in memmory
        //if they are in memmory they must be in DataStore as well and
        //the last_modified must be the same
        if (m_featureMap.contains(datasetId) && m_featureListMap.contains(datasetId)) {
            return request;
        }

        //if not in memmory it must be in the local cache (DataStore)
        if (!parseFeaturesJSONfile(datasetId)) {
            //something went wrong opening the cached file
            //TODO add Error to the request
            qDebug() << "[DataProxy] Something went wrong opening the cached JSON features for "
                        "dataset " << datasetId;
            request.return_code(async::DataRequest::CodeError);
        }

        return request;
    }

    //creates the request
    NetworkCommand *cmd =
            RESTCommandFactory::getFeatureByDatasetId(m_configurationManager, datasetId);
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    //QUuid encloses its uuids in "{}"
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36));
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(FeatureDataType)));
    parameters.insert(Globals::PARAM_DATASET, QVariant(datasetId));
    NetworkReply *reply = m_networkManager->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    //returns the request
    return createRequest(reply);
}

bool DataProxy::hasImageAlignment(const QString& imageAlignmentId) const
{
    const qlonglong last_modified = getImageAlignmentLastModified(imageAlignmentId);
    if (last_modified == -1) {
        qDebug() << "There was an error retrieving the "
                 << "last_modified for Image Alignment " << imageAlignmentId;
        return false;
    }

    return m_imageAlignmentMap.contains(imageAlignmentId)
            && m_imageAlignmentMap.value(imageAlignmentId)->lastModified().toLongLong() == last_modified;
}

async::DataRequest DataProxy::loadImageAlignmentById(const QString& imageAlignmentId)
{
    //check if present already
    if (hasImageAlignment(imageAlignmentId)) {
        return async::DataRequest(async::DataRequest::CodePresent);
    }

    //creates the request
    NetworkCommand *cmd =
            RESTCommandFactory::getImageAlignmentById(m_configurationManager, imageAlignmentId);
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    //QUuid encloses its uuids in "{}"
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36));
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(ImageAlignmentDataType)));
    NetworkReply *reply = m_networkManager->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    //returns the request
    return createRequest(reply);
}

async::DataRequest DataProxy::loadUser()
{
    //no need to check if present (we always reload the user)

    //creates the requet
    NetworkCommand *cmd = RESTCommandFactory::getUser(m_configurationManager);
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    //QUuid encloses its uuids in "{}"
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36));
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(UserType)));
    NetworkReply *reply = m_networkManager->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    //returns the request
    return createRequest(reply);
}

async::DataRequest DataProxy::loadGeneSelections()
{
    //no need to check if present (we always reload the selections for now)

    //creates the requet
    NetworkCommand* cmd = RESTCommandFactory::getSelections(m_configurationManager);
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    //QUuid encloses its uuids in "{}"
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36));
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(GeneSelectionDataType)));
    NetworkReply *reply = m_networkManager->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    //returns the request
    return createRequest(reply);
}

async::DataRequest DataProxy::updateGeneSelection(GeneSelectionPtr geneSelection)
{
    //To avoid any type of race conditions, we enforce
    //to peform a GET of the selections after updating one of them.
    //Currently the selections are not cached

    // intermediary dto object
    GeneSelectionDTO dto(*geneSelection);
    NetworkCommand *cmd =
            RESTCommandFactory::upateSelectionBySelectionId(m_configurationManager,
                                                            geneSelection->id());
    //append json data
    cmd->setJsonQuery(dto.toJson());
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    //QUuid encloses its uuids in "{}"
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36));
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(None)));
    NetworkReply *reply = m_networkManager->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    //return the request
    return createRequest(reply);
}

async::DataRequest DataProxy::addGeneSelection(const GeneSelection &geneSelection)
{
    //the new generate selection will be returned in the network reply but
    //we don't need to parse as the selections will be retrieved once we enter
    //the analysis view

    // intermediary dto object
    GeneSelectionDTO dto(geneSelection);
    NetworkCommand *cmd = RESTCommandFactory::addSelection(m_configurationManager);
    //append json data
    cmd->setJsonQuery(dto.toJson());
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    //QUuid encloses its uuids in "{}"
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36));
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(None)));
    NetworkReply *reply = m_networkManager->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    //return the request
    return createRequest(reply);
}

async::DataRequest DataProxy::removeSelection(const QString &selectionId)
{
    //To avoid any type of race conditions, it is enforce
    //to peform a GET of the selections after removing one of them.
    //Currently the selections are not cached

    NetworkCommand *cmd =
            RESTCommandFactory::removeSelectionBySelectionId(m_configurationManager, selectionId);
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    //QUuid encloses its uuids in "{}"
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36));
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(None)));
    NetworkReply *reply = m_networkManager->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    //return the request
    return createRequest(reply);
}

//last modified will be added to the resource store if not present
bool DataProxy::hasCellTissue(const QString& name)
{
    const qlonglong last_modified = getImageFileLastModified(name);
    if (last_modified == -1) {
        qDebug() << "There was an error retrieving the last_modified for image " << name;
        return false;
    }

    return m_dataStore.hasResource(name) &&
            !m_dataStore.resourceIsModified(name, last_modified);
}

async::DataRequest DataProxy::loadCellTissueByName(const QString& name)
{
    //check if present already (cell tissue file should always be the same)
    if (hasCellTissue(name)) {
        return async::DataRequest(async::DataRequest::CodePresent);
    }

    //creates the request
    NetworkCommand *cmd =
            RESTCommandFactory::getCellTissueFigureByName(m_configurationManager, name);
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    //QUuid encloses its uuids in "{}"
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36));
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(TissueDataType)));
    parameters.insert(Globals::PARAM_FILE, QVariant(name));
    NetworkReply *reply = m_networkManager->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    return createRequest(reply);
}

async::DataRequest DataProxy::loadMinVersion()
{
    // check if the version is supported in the server and check for updates
    NetworkCommand *cmd = RESTCommandFactory::getMinVersion(m_configurationManager);
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(MinVersionType)));
    // send empty flags to ensure access token is not appended to request
    NetworkReply *reply =
            m_networkManager->httpRequest(cmd, QVariant(parameters), NetworkManager::Empty);
    //delete the command
    cmd->deleteLater();
    return createRequest(reply);
}

qlonglong DataProxy::getFeaturesLastModified(const QString& datasetId) const
{
    NetworkCommand *cmd =
            RESTCommandFactory::getFeaturesLastModified(m_configurationManager, datasetId);
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    //QUuid encloses its uuids in "{}"
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36));
    // send empty flags to ensure access token is not appended to request
    NetworkReply *reply =
            m_networkManager->httpRequest(cmd, QVariant(), NetworkManager::Empty);
    //delete the command
    cmd->deleteLater();
    return createAndParseRequestLastModified(reply);
}

qlonglong DataProxy::getImageFileLastModified(const QString& figureFile) const
{
    NetworkCommand *cmd =
            RESTCommandFactory::getImageLastModified(m_configurationManager, figureFile);
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    //QUuid encloses its uuids in "{}"
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36));
    // send empty flags to ensure access token is not appended to request
    NetworkReply *reply =
            m_networkManager->httpRequest(cmd, QVariant(), NetworkManager::Empty);
    //delete the command
    cmd->deleteLater();
    //parse reply
    return createAndParseRequestLastModified(reply);
}

qlonglong DataProxy::getImageAlignmentLastModified(const QString &imageAlignmentId) const
{
    NetworkCommand *cmd =
            RESTCommandFactory::getImageAlignmentLastModified(m_configurationManager, imageAlignmentId);
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    //QUuid encloses its uuids in "{}"
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36));
    // send empty flags to ensure access token is not appended to request
    NetworkReply *reply =
            m_networkManager->httpRequest(cmd, QVariant(), NetworkManager::Empty);
    //delete the command
    cmd->deleteLater();
    //parse reply
    return createAndParseRequestLastModified(reply);
}

qlonglong DataProxy::getChipLastModified(const QString &chipId) const
{
    NetworkCommand *cmd =
            RESTCommandFactory::getChipLastModified(m_configurationManager, chipId);
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    //QUuid encloses its uuids in "{}"
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36));
    // send empty flags to ensure access token is not appended to request
    NetworkReply *reply =
            m_networkManager->httpRequest(cmd, QVariant(), NetworkManager::Empty);
    //delete the command
    cmd->deleteLater();
    //parse reply
    return createAndParseRequestLastModified(reply);
}

qlonglong DataProxy::getDatasetLastModified(const QString &datasetId) const
{
    NetworkCommand *cmd =
            RESTCommandFactory::getDatasetLastModified(m_configurationManager, datasetId);
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    //QUuid encloses its uuids in "{}"
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36));
    // send empty flags to ensure access token is not appended to request
    NetworkReply *reply =
            m_networkManager->httpRequest(cmd, QVariant(), NetworkManager::Empty);
    //delete the command
    cmd->deleteLater();
    //parse reply
    return createAndParseRequestLastModified(reply);
}

//TODO duplicated code in createRequest()
qlonglong DataProxy::createAndParseRequestLastModified(NetworkReply *reply) const
{
    if (reply == nullptr) {
        return -1;
    }

    //Well, the reason to use an eventloop here is to make the network request, synchronous.
    //A better solution would be to use threads and parse the reply in a different slot.
    //In order to avoid possible problems in the main UI event loop some
    //flags are sent in the exec()
    QEventLoop loop;
    connect(reply, SIGNAL(signalFinished(QVariant, QVariant)), &loop, SLOT(quit()));
    loop.exec(QEventLoop::ExcludeUserInputEvents
              | QEventLoop::X11ExcludeTimers | QEventLoop::WaitForMoreEvents);

    //TODO refactor this a bit
    qlonglong returnValue;
    if (reply->hasErrors()) {
        returnValue = -1;
    } else {
        const NetworkReply::ReturnCode returnCode =
                static_cast<NetworkReply::ReturnCode>(reply->return_code());

        if (returnCode != NetworkReply::CodeError && returnCode != NetworkReply::CodeAbort) {
            const QJsonDocument doc = reply->getJSON();
            if (doc.isNull() || doc.isEmpty()) {
                returnValue = -1;
            } else {
                // intermediary parse object
                LastModifiedDTO dto;
                // should only be one item
                const QVariant root = doc.toVariant();
                data::parseObject(root, &dto);
                returnValue = dto.lastModified().toLongLong();
            }
        } else {
            returnValue = -1;
        }
    }
    //reply has been processed, lets delete it
    reply->deleteLater();
    //return last modified value
    return returnValue;
}

async::DataRequest DataProxy::createRequest(NetworkReply *reply)
{
    if (reply == nullptr) {
        qDebug() << "[DataProxy] : Error, the NetworkReply is null,"
                    "there must have been a network error";
        return async::DataRequest(async::DataRequest::CodeError);
    }

    //Well, the reason to use an eventloop here is to make the network request, synchronous.
    //A better solution would be to use threads and parse the reply in a different slot.
    //In order to avoid possible problems in the main UI event loop some
    //flags are sent in the exec()
    QEventLoop loop;
    connect(reply, SIGNAL(signalFinished(QVariant, QVariant)), &loop, SLOT(quit()));
    loop.exec(QEventLoop::ExcludeUserInputEvents
              | QEventLoop::X11ExcludeTimers | QEventLoop::WaitForMoreEvents);

    //TODO add slot Abort to DataRequest and connect it to the reply
    //to allow to abort requests trough the progress bar dialog

    async::DataRequest request;

    if (reply->hasErrors()) {
        request.addError(reply->parseErrors());
        request.return_code(async::DataRequest::CodeError);
    } else {
        const NetworkReply::ReturnCode returnCode =
                static_cast<NetworkReply::ReturnCode>(reply->return_code());

        if (returnCode == NetworkReply::CodeError) {
            //strange..the reply does not have registered errors but yet
            //the returned code is ERROR
            QSharedPointer<Error>
                    error(new Error("Data Error", "There was an error downloading data", this));
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
            parseData(reply, parameters);
            //TODO parseData() returns false if no data was parsed...what to do?
            request.return_code(async::DataRequest::CodeSuccess);

            //errors could happen parsing the data
            if (reply->hasErrors()) {
                request.addError(reply->parseErrors());
                request.return_code(async::DataRequest::CodeError);
            }
        }
    }

    //reply has been processed, lets delete it
    reply->deleteLater();

    //return request
    return request;
}

//TODO need to parse genes here
bool DataProxy::parseFeaturesJSONfile(const QString& datasetId)
{
    Q_ASSERT(!datasetId.isEmpty() && !datasetId.isNull());
    bool dirty = false;

    DataStore::resourceDeviceType device =
            m_dataStore.accessResource(datasetId, resourceFlags);
    Q_ASSERT(!device->isOpen());

    const bool dataOpen = device->open(QIODevice::ReadOnly);
    if (!dataOpen) {
        return false;
    }

    //get the raw JSON and create JSON document
    QByteArray rawJSON = device->readAll();
    Q_ASSERT(!rawJSON.isEmpty() && !rawJSON.isNull());
    QJsonDocument doc = QJsonDocument::fromJson(rawJSON);
    // validate JSON document
    if (doc.isNull() || doc.isEmpty()) {
        return false;
    }

    //get the genes containers and clear them
    GeneList& geneListByDatasetId = getGeneList(datasetId);
    GeneMap& geneMapByDatasetId = getGeneMap(datasetId);
    //clear the data
    geneListByDatasetId.clear();
    geneMapByDatasetId.clear();

    // intermediary parse object and end object map
    FeatureDTO dto;
    FeatureList& featureListByDatasetId = getFeatureList(datasetId);
    FeatureMap& featureMapByDatasetId = getFeatureMap(datasetId);
    //clear the data
    featureListByDatasetId.clear();
    featureMapByDatasetId.clear();
    // ensure even single items are encapsulated in a variant list
    const QVariant root = doc.toVariant();
    const QVariantList list = root.canConvert(QVariant::List) ? root.toList()
                                                              : (QVariantList() += root);
    //parse the data
    foreach(QVariant var, list) {
        data::parseObject(var, &dto);
        FeaturePtr feature = FeaturePtr(new Feature(dto.feature()));
        FeatureList& featureListByGeneIdAndDatasetId =
                getGeneFeatureList(datasetId, feature->gene());

        //TODO optimize this
        //TODO no need of extra containers for genes probably
        if (geneMapByDatasetId.contains(feature->gene())) {
            feature->geneObject(geneMapByDatasetId.value(feature->gene()));
        } else {
            GenePtr gene = GenePtr(new Gene(feature->id(), feature->gene()));
            geneListByDatasetId.push_back(gene);
            geneMapByDatasetId.insert(gene->name(), gene);
            feature->geneObject(gene);
        }

        //TODO clear featureListByGeneIdAndDatasetId (check if this is consistent, can prob be removed)
        featureMapByDatasetId.insert(feature->id(), feature);
        featureListByGeneIdAndDatasetId.push_back(feature);
        featureListByDatasetId.push_back(feature);
        dirty = true;
    }

    return dirty;
}
