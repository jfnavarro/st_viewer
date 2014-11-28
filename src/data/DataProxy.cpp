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
#include <QImage>
#include <QMessageBox>
#include <QImageReader>

#include "config/Configuration.h"
#include "network/NetworkManager.h"
#include "network/NetworkCommand.h"
#include "network/NetworkReply.h"
#include "network/RESTCommandFactory.h"
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

#include "rapidjson/reader.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/error/en.h"

#include <iostream>
#include <sstream>

using namespace rapidjson;

//Handler with call backs for rapidjson
//explicitly made to parse the Features JSON type object
//references to the containers are passed and will be filled up
//with the parsed objects
//TODO move to a separate file
struct FeaturesHanlder {

public:

    FeaturesHanlder(DataProxy::FeatureList &m_featuresList,
                    DataProxy::GeneList &m_genesList,
                    DataProxy::GeneFeatureMap &m_geneFeaturesMap)
        : featuresList(m_featuresList),
          genesList(m_genesList),
          geneFeaturesMap(m_geneFeaturesMap)
    {
    }

    bool Null() { return false; }
    bool Bool(bool) { return false; }

    bool Int(int i)
    {
        varMap.insert(currentKey, i);
        return true;
    }

    bool Uint(unsigned u)
    {
        varMap.insert(currentKey, u);
        return true;
    }


    bool Int64(int64_t i)
    {
        varMap.insert(currentKey, static_cast<qlonglong>(i));
        return true;
    }

    bool Uint64(uint64_t u)
    {
        varMap.insert(currentKey, static_cast<qulonglong>(u));
        return true;
    }

    bool Double(double d)
    {
        varMap.insert(currentKey, d);
        return true;
    }

    bool String(const char* str, SizeType length, bool)
    {
        std::string std_str(str, length);
        varMap.insert(currentKey, QString::fromStdString(std_str));
        return true;
    }

    bool StartObject()
    {
        varMap.clear();
        currentKey.clear();
        return true;
    }

    bool Key(const char* str, SizeType length, bool)
    {
        std::string std_str(str, length);
        currentKey = QString::fromStdString(std_str);
        return true;
    }

    bool EndObject(SizeType)
    {
        //create feature object from variant map
        FeatureDTO dto;
        data::parseObject(varMap, &dto);
        DataProxy::FeaturePtr feature = DataProxy::FeaturePtr(new Feature(dto.feature()));

        //get the gene name
        const QString gene = feature->gene();

        //create unique gene object
        DataProxy::GenePtr genePtr;
        if (geneNameToGene.contains(gene)) {
            genePtr = geneNameToGene.value(gene);
        } else {
            genePtr =  DataProxy::GenePtr(new Gene(gene));
            geneNameToGene.insert(gene, genePtr);
            genesList.push_back(genePtr);
        }

        //update feature with the unique gene object
        feature->geneObject(genePtr);

        //update containers
        featuresList.push_back(feature);
        geneFeaturesMap.insert(gene, feature);

        return true;
    }

    bool StartArray() { return true; }
    bool EndArray(SizeType) { return true; }

private:

    DataProxy::FeatureList &featuresList;
    DataProxy::GeneList &genesList;
    DataProxy::GeneFeatureMap &geneFeaturesMap;
    QHash<QString, DataProxy::GenePtr> geneNameToGene;
    QString currentKey;
    QVariantMap varMap;
};

DataProxy::DataProxy(QObject *parent) :
    QObject(parent),
    m_user(nullptr),
    m_networkManager(nullptr),
    m_activeDownloads(0)
{
    //initialize data containers
    m_user = UserPtr(new User());

    m_networkManager = new NetworkManager(this);
    Q_ASSERT(!m_networkManager.isNull());
}

DataProxy::~DataProxy()
{
    m_networkManager->deleteLater();
    m_networkManager = nullptr;

    m_user.clear();

    clean();
}

void DataProxy::clean()
{
    qDebug() << "Cleaning memory cache in Dataproxy";

    //every data member is a smart pointer
    //TODO make totally sure data is being de-allocated

    m_datasetList.clear();
    m_geneSelectionsList.clear();
    m_imageAlignment.clear();
    m_chip.clear();
    m_featuresList.clear();
    m_genesList.clear();
    m_geneFeaturesMap.clear();
    m_cellTissueImages.clear();
    //m_minVersion.clear();
    //m_accessToken.clear();
    m_selectedDataset.clear();
    m_activeDownloads = 0;
    m_activeNetworkReplies.clear();
}

void DataProxy::cleanAll()
{
    qDebug() << "Cleaning memory cache and disk cache in Dataproxy";
    clean();
    m_networkManager->cleanCache();
}

const DataProxy::DatasetList& DataProxy::getDatasetList() const
{
    return m_datasetList;
}

const DataProxy::DatasetPtr DataProxy::getDatasetById(const QString& datasetId) const
{
    DataProxy::DatasetPtr foundDataset;
    //dataset list will always be short so a simple find function is enough
    foreach(DataProxy::DatasetPtr dataset, m_datasetList) {
        if (dataset->id() == datasetId) {
            foundDataset = dataset;
            break;
        }
    }

    return foundDataset;
}

const DataProxy::GeneList& DataProxy::getGeneList() const
{
    return m_genesList;
}

const DataProxy::FeatureList& DataProxy::getFeatureList() const
{
    return m_featuresList;
}

const DataProxy::FeatureList DataProxy::getGeneFeatureList(const QString &geneName) const
{
    return m_geneFeaturesMap.contains(geneName) ?
                m_geneFeaturesMap.values(geneName) : FeatureList();
}

DataProxy::UserPtr DataProxy::getUser() const
{
    return m_user;
}

DataProxy::ChipPtr DataProxy::getChip() const
{
    return m_chip;
}

DataProxy::ImageAlignmentPtr DataProxy::getImageAlignment() const
{
    return m_imageAlignment;
}

const QByteArray DataProxy::getFigureRed() const
{
    Q_ASSERT(!m_imageAlignment.isNull()
             && !m_imageAlignment->figureRed().isNull()
             && !m_imageAlignment->figureRed().isEmpty());
    return m_cellTissueImages.value(m_imageAlignment->figureRed());
}

const QByteArray DataProxy::getFigureBlue() const
{
    Q_ASSERT(!m_imageAlignment.isNull()
             && !m_imageAlignment->figureBlue().isNull()
             && !m_imageAlignment->figureBlue().isEmpty());
    return m_cellTissueImages.value(m_imageAlignment->figureBlue());
}

const DataProxy::GeneSelectionList DataProxy::getGenesSelectionsList() const
{
    return m_geneSelectionsList;
}

const DataProxy::DatasetPtr DataProxy::getSelectedDataset() const
{
    //used to keep track of what dataset is currently selected
    return m_selectedDataset;
}

void DataProxy::setSelectedDataset(const DatasetPtr dataset) const
{
    //used to keep track of what dataset is currently selected
    m_selectedDataset = dataset;
}

void DataProxy::resetSelectedDataset()
{
    m_selectedDataset.clear();
}

const DataProxy::MinVersionArray DataProxy::getMinVersion() const
{
    return m_minVersion;
}

const OAuth2TokenDTO DataProxy::getAccessToken() const
{
    return m_accessToken;
}

void DataProxy::loadDatasets()
{
    //clean up container
    m_datasetList.clear();
    //creates the request
    NetworkCommand *cmd = RESTCommandFactory::getDatasets(m_configurationManager);
    NetworkReply *reply = m_networkManager->httpRequest(cmd);
    //delete the command
    cmd->deleteLater();
    //activate the download
    createRequest(reply, &DataProxy::signalDatasetsDownloaded, &DataProxy::parseDatasets);
}

void DataProxy::updateDataset(const Dataset &dataset)
{
    // intermediary dto object
    DatasetDTO dto(dataset);
    NetworkCommand *cmd =
            RESTCommandFactory::updateDatasetByDatasetId(m_configurationManager, dataset.id());
    //append json data
    cmd->setBody(dto.toJson());
    NetworkReply *reply = m_networkManager->httpRequest(cmd);
    //delete the command
    cmd->deleteLater();
    //activate the download
    createRequest(reply, &DataProxy::signalDatasetsModified);
}

void DataProxy::removeDataset(const QString& datasetId)
{
    NetworkCommand *cmd =
            RESTCommandFactory::removeDatasetByDatasetId(m_configurationManager, datasetId);
    NetworkReply *reply = m_networkManager->httpRequest(cmd);
    //add dataset ID to the reply metaproperty
    reply->setProperty("dataset_id", QVariant::fromValue<QString>(datasetId));
    //delete the command
    cmd->deleteLater();
    //activate the download
    createRequest(reply, &DataProxy::signalDatasetsModified, &DataProxy::parseRemoveDataset);
}

void DataProxy::loadDatasetContent()
{
    Q_ASSERT(!m_selectedDataset.isNull());
    Q_ASSERT(!m_imageAlignment.isNull());

    //TODO this can cause a race condition if any of the downloads
    //is completed before creating the next one

    //load cell tissue blue
    loadCellTissueByName(m_imageAlignment->figureBlue());
    Q_ASSERT(!m_user.isNull());
    if (m_user->hasSpecialRole()) {
        //load cell tissue red (no need to download for role USER)
        loadCellTissueByName(m_imageAlignment->figureRed());
    }

    //load features
    loadFeatures();

    //load chip
    loadChip();
}

void DataProxy::loadChip()
{
    Q_ASSERT(!m_imageAlignment.isNull()
             && !m_imageAlignment->chipId().isNull()
             && !m_imageAlignment->chipId().isEmpty());
    const QString chipId = m_imageAlignment->chipId();
    //clear container
    m_chip.clear();
    //creates the request
    NetworkCommand *cmd =
            RESTCommandFactory::getChipByChipId(m_configurationManager, chipId);
    NetworkReply *reply = m_networkManager->httpRequest(cmd);
    //delete the command
    cmd->deleteLater();
    //activate the download
    createRequest(reply, &DataProxy::signalDatasetContentDownloaded, &DataProxy::parseChip);
}

void DataProxy::loadFeatures()
{
    Q_ASSERT(!m_selectedDataset.isNull()
             && !m_selectedDataset->id().isNull()
             && !m_selectedDataset->id().isEmpty());
    // clear the containers
    m_genesList.clear();
    m_featuresList.clear();
    m_geneFeaturesMap.clear();
    const QString datasetId = m_selectedDataset->id();
    //creates the request
    NetworkCommand *cmd =
            RESTCommandFactory::getFeatureByDatasetId(m_configurationManager, datasetId);
    NetworkReply *reply = m_networkManager->httpRequest(cmd);
    //delete the command
    cmd->deleteLater();
    //activate the download
    createRequest(reply, &DataProxy::signalDatasetContentDownloaded, &DataProxy::parseFeatures);
}

void DataProxy::loadImageAlignment()
{
    Q_ASSERT(!m_selectedDataset.isNull()
             && !m_selectedDataset->imageAlignmentId().isNull()
             && !m_selectedDataset->imageAlignmentId().isEmpty());
    const QString imageAlignmentId = m_selectedDataset->imageAlignmentId();
    //clear container
    m_imageAlignment.clear();
    //creates the request
    NetworkCommand *cmd =
            RESTCommandFactory::getImageAlignmentById(m_configurationManager, imageAlignmentId);
    NetworkReply *reply = m_networkManager->httpRequest(cmd);
    //delete the command
    cmd->deleteLater();
    //activate the download
    createRequest(reply, &DataProxy::signalImageAlignmentDownloaded,
                  &DataProxy::parseImageAlignment);
}

void DataProxy::loadUser()
{
    //clear container
    m_user.clear();
    //creates the requet
    NetworkCommand *cmd = RESTCommandFactory::getUser(m_configurationManager);
    NetworkReply *reply = m_networkManager->httpRequest(cmd);
    //delete the command
    cmd->deleteLater();
    //activate the download
    createRequest(reply, &DataProxy::signalUserDownloaded, &DataProxy::parseUser);
}

void DataProxy::loadGeneSelections()
{
    //clean up the containers
    m_geneSelectionsList.clear();
    //creates the requet
    NetworkCommand* cmd = RESTCommandFactory::getSelections(m_configurationManager);
    NetworkReply *reply = m_networkManager->httpRequest(cmd);
    //delete the command
    cmd->deleteLater();
    //activate the download
    createRequest(reply, &DataProxy::signalGenesSelectionsDownloaded,
                  &DataProxy::parseGeneSelections);
}

void DataProxy::updateGeneSelection(const GeneSelection &geneSelection)
{
    // intermediary dto object
    GeneSelectionDTO dto(geneSelection);
    NetworkCommand *cmd =
            RESTCommandFactory::upateSelectionBySelectionId(m_configurationManager,
                                                            geneSelection.id());
    //append json data
    cmd->setBody(dto.toJson());
    NetworkReply *reply = m_networkManager->httpRequest(cmd);
    //delete the command
    cmd->deleteLater();
    //activate the download
    createRequest(reply, &DataProxy::signalGenesSelectionModified);
}

void DataProxy::addGeneSelection(const GeneSelection &geneSelection)
{
    // intermediary dto object
    GeneSelectionDTO dto(geneSelection);
    NetworkCommand *cmd = RESTCommandFactory::addSelection(m_configurationManager);
    //append json data
    cmd->setBody(dto.toJson());
    NetworkReply *reply = m_networkManager->httpRequest(cmd);
    //delete the command
    cmd->deleteLater();
    //activate the download
    createRequest(reply);
}

void DataProxy::removeSelection(const QString &selectionId)
{
    NetworkCommand *cmd =
            RESTCommandFactory::removeSelectionBySelectionId(m_configurationManager, selectionId);
    NetworkReply *reply = m_networkManager->httpRequest(cmd);
    //delete the command
    cmd->deleteLater();
    //activate the download
    createRequest(reply, &DataProxy::signalGenesSelectionModified);
}

void DataProxy::loadCellTissueByName(const QString& name)
{
    //remove image if already exists and add the newly created one
    if (m_cellTissueImages.contains(name)) {
        m_cellTissueImages.remove(name);
    }
    //creates the request
    NetworkCommand *cmd =
            RESTCommandFactory::getCellTissueFigureByName(m_configurationManager, name);
    NetworkReply *reply = m_networkManager->httpRequest(cmd);
    //add figure name to reply metaproperty
    reply->setProperty("figure_name", QVariant::fromValue<QString>(name));
    //delete the command
    cmd->deleteLater();
    //activate the download
    createRequest(reply, &DataProxy::signalDatasetContentDownloaded,
                  &DataProxy::parseCellTissueImage);
}

void DataProxy::loadMinVersion()
{
    NetworkCommand *cmd = RESTCommandFactory::getMinVersion(m_configurationManager);
    // send empty flags to ensure access token is not appended to request
    NetworkReply *reply = m_networkManager->httpRequest(cmd, NetworkManager::Empty);
    //delete the command
    cmd->deleteLater();
    //activate the download
    createRequest(reply, &DataProxy::signalMinVersionDownloaded, &DataProxy::parseMinVersion);
}

void DataProxy::loadAccessToken(const StringPair &username,
                                const StringPair &password)
{
    NetworkCommand *cmd = RESTCommandFactory::getAuthorizationToken(m_configurationManager);
    //add username and password to the request
    cmd->addQueryItem(username.first, username.second);
    cmd->addQueryItem(password.first, password.second);

    // send empty flags to ensure access token is not appended to request
    NetworkReply *reply = m_networkManager->httpRequest(cmd, NetworkManager::Empty);
    //delete the command
    cmd->deleteLater();
    //activate the download
    createRequest(reply, &DataProxy::signalAccessTokenDownloaded, &DataProxy::parseOAuth2);
}

void DataProxy::createRequest(NetworkReply *reply,void (DataProxy::*signal)(DownloadStatus), bool
                              (DataProxy::*parseFunc)(NetworkReply *reply))
{
    if (reply == nullptr) {
        qDebug() << "[DataProxy] : Error, the NetworkReply is null,"
                    "there must have been a network error";
        QMessageBox::critical(nullptr, tr("Error downloading data"),
                              tr("There was probably a network problem."));
        if (signal != nullptr) {
            emit (this->*signal)(DataProxy::Failed);
        }
    }

    // increase active downloads counter
    m_activeDownloads++;
    // add reply to the active download replies
    m_activeNetworkReplies.insert(reply, QPair<void (DataProxy::*)(DataProxy::DownloadStatus),
                                  bool (DataProxy::*)(NetworkReply *reply)>(signal, parseFunc));
    // connect parsing function to reply
    connect(reply, SIGNAL(signalFinished(QVariant)), this, SLOT(slotProcessDownload()));
}

void DataProxy::slotProcessDownload()
{
    //get reply object from the caller
    NetworkReply *reply = qobject_cast<NetworkReply*>(sender());
    Q_ASSERT(reply != nullptr);

    //get the parse function from the container (can be nullptr)
    const auto parseFuncSignalPair = m_activeNetworkReplies.value(reply);
    const auto parseFunc = parseFuncSignalPair.second;
    const auto parseSignal = parseFuncSignalPair.first;

    DataProxy::DownloadStatus status = DataProxy::Failed;

    if (!reply->hasErrors()) {
        const NetworkReply::ReturnCode returnCode =
                static_cast<NetworkReply::ReturnCode>(reply->return_code());

        if (returnCode == NetworkReply::CodeError) {
            //strange..the reply does not have registered errors but yet
            //the returned code is ERROR
            QMessageBox::critical(nullptr, tr("Error downloading data"),
                                  tr("There was probably a network problem."));
            status = DataProxy::Failed;
        } else if (returnCode == NetworkReply::CodeAbort) {
            status = DataProxy::Aborted;
        } else {
            bool parsedOk = true;
            if (parseFunc != nullptr) {
                parsedOk = (this->*parseFunc)(reply);
            }

            //errors could happen parsing the data
            if (reply->hasErrors() || !parsedOk) {
                const auto error = reply->parseErrors();
                QMessageBox::critical(nullptr, error->name(), error->description());
                status = DataProxy::Failed;
            }

            status = DataProxy::Success;
        }
    } else {
        const auto error = reply->parseErrors();
        QMessageBox::critical(nullptr, error->name(), error->description());
    }

    //reply has been processed, lets delete it and decrease counters
    if (m_activeDownloads > 0) {
        m_activeDownloads--;
    } else {
        qDebug() << "[DataProxy] A network reply call back has been invoked with no active "
            "downloads.";
    }
    const bool removedOK = m_activeNetworkReplies.remove(reply);
    //TODO there is a race condition triggering this assertion,
    //the call back is called twice so the second time there is nothing to remove
    //a trello ticket is created for this and will be solved soon
    if (!removedOK) {
        qDebug() << "[DataProxy] A network reply call back has been invoked but it has been "
                    "processed earlier already.";
    }
    reply->deleteLater();

    //check if last download
    if (m_activeDownloads == 0 && parseSignal != nullptr) {
        emit (this->*parseSignal)(status);
    }
}

void DataProxy::slotAbortActiveDownloads()
{
    foreach(NetworkReply *reply, m_activeNetworkReplies.keys()){
        //abort will trigger the process signal which will to the function above
        //being called, perhaps change that logic
        reply->slotAbort();
    }

}

bool DataProxy::parseFeatures(NetworkReply *reply)
{
    bool dirty = true;
    FeaturesHanlder handler(m_featuresList, m_genesList, m_geneFeaturesMap);
    Reader reader;
    QByteArray rawText = reply->getRaw();
    StringStream is(rawText.data());
    const bool ok = reader.Parse(is, handler);
    if (!ok) {
        ParseErrorCode e = reader.GetParseErrorCode();
        qDebug() << "[DataProxy] Error parsing features JSON : " << GetParseError_En(e) << endl;
        dirty = false;
    }
    return dirty;
}

bool DataProxy::parseCellTissueImage(NetworkReply *reply)
{
    // get filename and file raw data (check if it was encoded or not)
    const QByteArray &rawImage = reply->getRaw();
    const bool imageOk = !rawImage.isEmpty() && !rawImage.isNull();

    const QString &imageName = reply->property("figure_name").toString();
    const bool nameOk = !imageName.isEmpty() && !imageName.isNull();

    //store the image as raw data
    m_cellTissueImages.insert(imageName, rawImage);

    return imageOk && nameOk;
}

bool DataProxy::parseDatasets(NetworkReply *reply)
{
    const QJsonDocument &doc = reply->getJSON();
    if (doc.isNull() || doc.isEmpty()) {
        return false;
    }

    // intermediary parse object
    DatasetDTO dto;

    bool dirty = false;

    // parse the objects
    foreach(QVariant var, doc.toVariant().toList()) {
        data::parseObject(var, &dto);
        DatasetPtr dataset = DatasetPtr(new Dataset(dto.dataset()));
        m_datasetList.push_back(dataset);
        dirty = true;
    }

    return dirty;
}

bool DataProxy::parseRemoveDataset(NetworkReply *reply)
{
    const QString &datasetId = reply->property("dataset_id").toString();
    //just to make sure to reset the currently selected dataset variable if we
    //are removing the selected dataset
    if (!m_selectedDataset.isNull() && datasetId == m_selectedDataset->id()) {
        resetSelectedDataset();
    }

    return true;
}

bool DataProxy::parseGeneSelections(NetworkReply *reply)
{
    const QJsonDocument &doc = reply->getJSON();
    if (doc.isNull() || doc.isEmpty()) {
        return false;
    }

    // intermediary parse object
    GeneSelectionDTO dto;

    bool dirty = false;

    //parse the data
    foreach(QVariant var, doc.toVariant().toList()) {
        data::parseObject(var, &dto);
        GeneSelectionPtr selection = GeneSelectionPtr(new GeneSelection(dto.geneSelection()));
        //get the dataset name from the cached datasets
        //TODO if we allow to enter the Analysis View without having entered
        //the Datasets View this will fail. In that case, the dataset has to be retrieved
        //from the network by its ID
        const DatasetPtr dataset = getDatasetById(selection->datasetId());
        if (dataset.isNull()) {
            return false;
        }
        selection->datasetName(dataset->name());
        m_geneSelectionsList.push_back(selection);
        dirty = true;
    }

    return dirty;
}

bool DataProxy::parseUser(NetworkReply *reply)
{
    const QJsonDocument &doc = reply->getJSON();
    if (doc.isNull() || doc.isEmpty()) {
        return false;
    }

    // intermediary parse object
    UserDTO dto;

    // should only be one item
    const QVariant root = doc.toVariant();
    data::parseObject(root, &dto);
    m_user = UserPtr(new User(dto.user()));
    return true;
}

bool DataProxy::parseImageAlignment(NetworkReply *reply)
{
    const QJsonDocument &doc = reply->getJSON();
    if (doc.isNull() || doc.isEmpty()) {
        return false;
    }

    // intermediary parse object
    ImageAlignmentDTO dto;

    // image alignment should only contain one object
    const QVariant root = doc.toVariant();
    data::parseObject(root, &dto);
    ImageAlignmentPtr imageAlignement =
            ImageAlignmentPtr(new ImageAlignment(dto.imageAlignment()));
    m_imageAlignment = imageAlignement;
    return true;
}

bool DataProxy::parseChip(NetworkReply *reply)
{
    const QJsonDocument &doc = reply->getJSON();
    if (doc.isNull() || doc.isEmpty()) {
        return false;
    }

    // intermediary parse object
    ChipDTO dto;

    // should only be one item
    const QVariant root = doc.toVariant();
    data::parseObject(root, &dto);
    ChipPtr chip = ChipPtr(new Chip(dto.chip()));
    m_chip = chip;
    return true;
}

bool DataProxy::parseMinVersion(NetworkReply *reply)
{
    const QJsonDocument &doc = reply->getJSON();
    if (doc.isNull() || doc.isEmpty()) {
        return false;
    }

    // intermediary parse object
    MinVersionDTO dto;

    // should only be one item
    const QVariant root = doc.toVariant();
    data::parseObject(root, &dto);
    m_minVersion = dto.minVersionAsNumber();
    return true;
}

bool DataProxy::parseOAuth2(NetworkReply *reply)
{
    const QJsonDocument &doc = reply->getJSON();
    if (doc.isNull() || doc.isEmpty()) {
        return false;
    }

    // intermediary parse object
    OAuth2TokenDTO dto;

    // should only be one item
    const QVariant root = doc.toVariant();
    data::parseObject(root, &dto);
    m_accessToken = dto;
    return true;
}
