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
#include "error/NetworkError.h"
#include "network/DownloadManager.h"

// parse objects
#include "data/ObjectParser.h"
#include "dataModel/ChipDTO.h"
#include "dataModel/DatasetDTO.h"
#include "dataModel/GeneDTO.h"
#include "dataModel/FeatureDTO.h"
#include "dataModel/ImageAlignmentDTO.h"
#include "dataModel/UserDTO.h"
#include "dataModel/GeneSelectionDTO.h"

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
    return m_authorizationManager;
}

//TODO too big function (split)
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
        // intermediary parse object
        DatasetDTO dto;
        // ensure even single items are encapsulated in a variant list
        const QVariant root = doc.toVariant();
        const QVariantList list = root.canConvert(QVariant::List) ? root.toList() : (QVariantList() += root);
        //no need to clear the storage to allow updates
        //parse the objects
        foreach(QVariant var, list) {
            data::parseObject(var, &dto);
            DatasetPtr dataset = DatasetPtr(new Dataset(dto.dataset()));
            m_datasetMap.insert(dataset->id(), dataset);
            dirty = true;
        }
        break;
    }
        // gene
    case GeneDataType: {
        const QJsonDocument doc = reply->getJSON();
        // gene list by dataset
        Q_ASSERT_X(parameters.contains(Globals::PARAM_DATASET),
                   "DataProxy", "GeneData must include dataset parameter!");
        const QString datasetId =
                qvariant_cast<QString>(parameters.value(Globals::PARAM_DATASET));
        // intermediary parse object and end object map
        GeneDTO dto;
        GeneList& geneListByDatasetId = getGeneList(datasetId);
        GeneMap& geneMapByDatasetId = getGeneMap(datasetId);
        // ensure even single items are encapsulated in a variant list
        const QVariant root = doc.toVariant();
        const QVariantList list = root.canConvert(QVariant::List) ? root.toList() : (QVariantList() += root);
        //clear the data
        geneListByDatasetId.clear();
        geneMapByDatasetId.clear();
        //parse the data
        foreach(QVariant var, list) {
            data::parseObject(var, &dto);
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
        // intermediary parse object
        ImageAlignmentDTO dto;
        // ensure even single items are encapsulated in a variant list
        const QVariant root = doc.toVariant();
        const QVariantList list = root.canConvert(QVariant::List) ? root.toList() : (QVariantList() += root);
        //parse the data
        foreach(QVariant var, list) {
            data::parseObject(var, &dto);
            ImageAlignmentPtr imageAlignement =
                    ImageAlignmentPtr(new ImageAlignment(dto.imageAlignment()));
            m_imageAlignmentMap.insert(imageAlignement->id(), imageAlignement);
            dirty = true;
        }
        break;
    }
        // gene selection
    case GeneSelectionDataType: {
        const QJsonDocument doc = reply->getJSON();
        // intermediary parse object
        GeneSelectionDTO dto;
        // ensure even single items are encapsulated in a variant list
        const QVariant root = doc.toVariant();
        const QVariantList list = root.canConvert(QVariant::List) ? root.toList() : (QVariantList() += root);
        //parse the data
        foreach(QVariant var, list) {
            data::parseObject(var, &dto);
            GeneSelectionPtr selection = GeneSelectionPtr(new GeneSelection(dto.geneSelection()));
            //get the dataset name here (easier)
            //TODO selections whose dataset is disabled will not be added
            //but a more advance implementation is needed so the selections
            //are always sync to the datasets
            DatasetPtr selectionDataset = getDatasetById(selection->datasetId());
            Q_ASSERT(!selectionDataset.isNull());
            selection->enabled(selectionDataset->enabled() && selection->enabled());
            selection->datasetName(selectionDataset->name());
            m_geneSelectionsMap.insert(selection->id(), selection);
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
        //clear the data
        featureListByDatasetId.clear();
        featureMapByDatasetId.clear();
        //parse the data
        foreach(QVariant var, list) {
            data::parseObject(var, &dto);
            FeaturePtr feature = FeaturePtr(new Feature(dto.feature()));
            FeatureList& featureListByGeneIdAndDatasetId =
                    getGeneFeatureList(datasetId, feature->gene());
            //TODO clear featureListByGeneIdAndDatasetId (check if this is consistent)
            featureMapByDatasetId.insert(feature->id(), feature);
            featureListByGeneIdAndDatasetId.push_back(feature);
            featureListByDatasetId.push_back(feature);
            dirty = true;
        }
        break;
    }
        // user
    case UserType: {
        const QJsonDocument doc = reply->getJSON();
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
                   "DataProxy", "Tissue must include file parameter!");
        const QString fileid = qvariant_cast<QString>(parameters.value(Globals::PARAM_FILE));
        DataStore::resourceDeviceType device;
        Q_ASSERT(!fileid.isNull() && !fileid.isEmpty());
        device = m_dataStore.accessResource(fileid,
                                            DataStore::Temporary |
                                            DataStore::Persistent |
                                            DataStore::Secure);
        // store data in file
        Q_ASSERT(!device->isOpen());
        const bool dataOpen = device->open(QIODevice::WriteOnly);
        if (!dataOpen) {
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
        // min supported version
    case MinVersionType: {
        const QJsonDocument doc = reply->getJSON();
        // intermediary parse object
        MinVersionDTO dto;
        // should only be one item
        const QVariant root = doc.toVariant();
        data::parseObject(root, &dto);
        m_minVersion = dto.minVersionAsNumber();
        qDebug() << "[stVi] Check min version min = "
                 << dto.minSupportedVersion() << " current = " << Globals::VERSION;
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

DataProxy::FeatureList &DataProxy::getGeneFeatureList(const QString& datasetId,
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

const DataProxy::DatasetList DataProxy::getDatasetList() const
{
    return m_datasetMap.values();
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

DataProxy::ImageAlignmentPtr DataProxy::getImageAlignment(const QString& imageAlignmentId)
{
    ImageAlignmentMap::const_iterator it = m_imageAlignmentMap.find(imageAlignmentId);
    ImageAlignmentMap::const_iterator end = m_imageAlignmentMap.end();
    if (it == end) {
        it = m_imageAlignmentMap.insert(imageAlignmentId,
                                        ImageAlignmentPtr(new ImageAlignment()));
    }
    return it.value();
}

DataStore::resourceDeviceType DataProxy::getFigure(const QString& figureId)
{
    return m_dataStore.accessResource(figureId,
                                      DataStore::Temporary |
                                      DataStore::Persistent |
                                      DataStore::Secure);
}

const DataProxy::GeneSelectionList DataProxy::getGeneSelections() const
{
    return m_geneSelectionsMap.values();
}

const QString DataProxy::getSelectedDataset() const
{
    return m_selected_datasetId;
}

const DataProxy::MinVersionArray DataProxy::getMinVersion() const
{
    return m_minVersion;
}

void DataProxy::setSelectedDataset(const QString &datasetId) const
{
    m_selected_datasetId = datasetId;
}

async::DataRequest DataProxy::loadDatasets()
{
    //NOTE not checking if there are datasets already loaded
    //it is safer to always force to download them

    //creates the request
    NetworkCommand *cmd = RESTCommandFactory::getDatasets(m_configurationManager);
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36)); // QUuid encloses its uuids in "{}"
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
    // intermediary dto object
    DatasetDTO dto(*dataset);
    NetworkCommand *cmd =
            RESTCommandFactory::updateDatsetByDatasetId(m_configurationManager, dataset->id());
    //append json data
    cmd->setJsonQuery(dto.toJson());
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36)); // QUuid encloses its uuids in "{}"
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(None)));
    NetworkReply *reply = m_networkManager->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    //return the request
    return createRequest(reply);
}


async::DataRequest DataProxy::loadGenesByDatasetId(const QString& datasetId)
{
    //NOTE not checking if there are datasets already loaded
    //it is safer to always force to download them

    //creates the request
    NetworkCommand *cmd = RESTCommandFactory::getGenesByDatasetId(m_configurationManager, datasetId);
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36)); // QUuid encloses its uuids in "{}"
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(GeneDataType)));
    parameters.insert(Globals::PARAM_DATASET, QVariant(datasetId));
    NetworkReply *reply = m_networkManager->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    //returns the request
    return createRequest(reply);
}

async::DataRequest DataProxy::loadChipById(const QString& chipId)
{
    //NOTE not checking if there are datasets already loaded
    //it is safer to always force to download them

    //creates the request
    NetworkCommand *cmd = RESTCommandFactory::getChipByChipId(m_configurationManager, chipId);
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36)); // QUuid encloses its uuids in "{}"
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(ChipDataType)));
    NetworkReply *reply = m_networkManager->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    //returns the request
    return createRequest(reply);
}

async::DataRequest DataProxy::loadFeatureByDatasetId(const QString& datasetId)
{
    //NOTE not checking if there are datasets already loaded
    //it is safer to always force to download them

    //creates the request
    NetworkCommand *cmd = RESTCommandFactory::getFeatureByDatasetId(m_configurationManager, datasetId);
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36)); // QUuid encloses its uuids in "{}"
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(FeatureDataType)));
    parameters.insert(Globals::PARAM_DATASET, QVariant(datasetId));
    NetworkReply *reply = m_networkManager->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    //returns the request
    return createRequest(reply);
}

async::DataRequest DataProxy::loadImageAlignmentById(const QString& imageAlignmentId)
{
    //NOTE not checking if there are datasets already loaded
    //it is safer to always force to download them

    //creates the request
    NetworkCommand *cmd =
            RESTCommandFactory::getImageAlignmentById(m_configurationManager, imageAlignmentId);
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36)); // QUuid encloses its uuids in "{}"
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
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36)); // QUuid encloses its uuids in "{}"
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
    //no need to check if present (we always reload the selections)

    //creates the requet
    NetworkCommand* cmd = RESTCommandFactory::getSelections(m_configurationManager);
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36)); // QUuid encloses its uuids in "{}"
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
    // intermediary dto object
    GeneSelectionDTO dto(*geneSelection);
    NetworkCommand *cmd =
            RESTCommandFactory::upateSelectionBySelectionById(m_configurationManager, geneSelection->id());
    //append json data
    cmd->setJsonQuery(dto.toJson());
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36)); // QUuid encloses its uuids in "{}"
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
    // intermediary dto object
    GeneSelectionDTO dto(geneSelection);
    NetworkCommand *cmd = RESTCommandFactory::addSelection(m_configurationManager);
    //append json data
    cmd->setJsonQuery(dto.toJson());
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36)); // QUuid encloses its uuids in "{}"
    QVariantMap parameters;
    parameters.insert(Globals::PARAM_TYPE, QVariant(static_cast<int>(GeneSelectionDataType)));
    NetworkReply *reply = m_networkManager->httpRequest(cmd, QVariant(parameters));
    //delete the command
    cmd->deleteLater();
    //return the request
    return createRequest(reply);
}

bool DataProxy::hasCellTissue(const QString& name) const
{
    return m_dataStore.hasResource(name);
}

async::DataRequest DataProxy::loadCellTissueByName(const QString& name)
{
    //check if present already (cell tissue file should always be the same)
    if (hasCellTissue(name)) {
        async::DataRequest request;
        request.return_code(async::DataRequest::CodePresent);
        return request;
    }
    //creates the request
    NetworkCommand *cmd = RESTCommandFactory::getCellTissueFigureByName(m_configurationManager, name);
    //append access token
    const QUuid accessToken = m_authorizationManager->getAccessToken();
    cmd->addQueryItem(QStringLiteral("access_token"), accessToken.toString().mid(1, 36)); // QUuid encloses its uuids in "{}"
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

async::DataRequest DataProxy::createRequest(NetworkReply *reply)
{
    async::DataRequest request;

    if (reply == nullptr) {
        qDebug() << "[DataPRoxy] : Error, the NetworkReply is null,"
                    "there must have been a network error";
        request.return_code(async::DataRequest::CodeError);
        return request;
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

    if (reply->hasErrors()) {
        request.addError(reply->parseErrors());
        request.return_code(async::DataRequest::CodeError);
    } else {
        const NetworkReply::ReturnCode returnCode =
                static_cast<NetworkReply::ReturnCode>(reply->return_code());

        if (returnCode == NetworkReply::CodeError) {
            //TODO use reply->getError() text message instead
            QSharedPointer<Error>
                    error(new Error("Data Error", "There was an error downloading data", nullptr));
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
                //TODO no data has been modified...what to do here?
            }
            request.return_code(async::DataRequest::CodeSuccess);
        }
    }

    //reply has been processed, lets delete it
    reply->deleteLater();

    //return request
    return request;
}
