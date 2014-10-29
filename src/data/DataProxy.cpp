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

DataProxy::DataProxy(QObject *parent) :
    QObject(parent),
    m_user(nullptr),
    m_networkManager(nullptr)
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

const QImage DataProxy::getFigureRed() const
{
    Q_ASSERT(!m_imageAlignment.isNull()
             && !m_imageAlignment->figureRed().isNull()
             && !m_imageAlignment->figureRed().isEmpty());
    return m_cellTissueImages.value(m_imageAlignment->figureRed());
}

const QImage DataProxy::getFigureBlue() const
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

async::DataRequest DataProxy::loadDatasets()
{
    //clean up container
    m_datasetList.clear();
    //creates the request
    NetworkCommand *cmd = RESTCommandFactory::getDatasets(m_configurationManager);
    NetworkReply *reply = m_networkManager->httpRequest(cmd);
    //delete the command
    cmd->deleteLater();
    //return the request
    return createRequest(reply, &DataProxy::parseDatasets);
}

async::DataRequest DataProxy::updateDataset(DatasetPtr dataset)
{
    // intermediary dto object
    DatasetDTO dto(*dataset);
    NetworkCommand *cmd =
            RESTCommandFactory::updateDatasetByDatasetId(m_configurationManager, dataset->id());
    //append json data
    cmd->setBody(dto.toJson());
    NetworkReply *reply = m_networkManager->httpRequest(cmd);
    //delete the command
    cmd->deleteLater();
    //return the request
    return createRequest(reply);
}

async::DataRequest DataProxy::removeDataset(const QString& datasetId)
{
    NetworkCommand *cmd =
            RESTCommandFactory::removeDatasetByDatasetId(m_configurationManager, datasetId);
    NetworkReply *reply = m_networkManager->httpRequest(cmd);
    //delete the command
    cmd->deleteLater();
    //return the request
    return createRequest(reply);
}

async::DataRequest DataProxy::loadDatasetContent()
{
    async::DataRequest request(async::DataRequest::CodeError);

    if (m_selectedDataset.isNull()){
        return request;
    }

    //load the image alignment first
    request = loadImageAlignment();
    if (!request.isSuccessFul()) {
        request.addError(QSharedPointer<Error>(new Error(tr("Data Loading Error"),
                                                         tr("Error loading the image alignment."),
                                                         this)));
        return request;
    }

    Q_ASSERT(!m_imageAlignment.isNull());
    //load cell tissue blue
    request = loadCellTissueByName(m_imageAlignment->figureBlue());
    if (!request.isSuccessFul()) {
        request.addError(QSharedPointer<Error>(new Error(tr("Data Loading Error"),
                                                         tr("Error loading the blue cell tissue image."),
                                                         this)));
        return request;
    }

    Q_ASSERT(!m_user.isNull());
    if (m_user->hasSpecialRole()) {
        //load cell tissue red (no need to download for role USER)
        request = loadCellTissueByName(m_imageAlignment->figureRed());
        if (!request.isSuccessFul()) {
            request.addError(QSharedPointer<Error>(new Error(tr("Data Loading Error"),
                                                             tr("Error loading the red cell tissue image."),
                                                             this)));
            return request;
        }
    }

    //load chip
    request = loadChip();
    if (!request.isSuccessFul()) {
        request.addError(QSharedPointer<Error>(new Error(tr("Data Loading Error"),
                                                         tr("Error loading the chip."),
                                                         this)));
        return request;
    }

    //load features
    request = loadFeatures();
    if (!request.isSuccessFul()) {
        request.addError(QSharedPointer<Error>(new Error(tr("Data Loading Error"),
                                                         tr("Error loading the features."),
                                                         this)));
        return request;
    }

    request.return_code(async::DataRequest::CodeSuccess);
    return request;
}

async::DataRequest DataProxy::loadChip()
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
    //returns the request
    return createRequest(reply, &DataProxy::parseChip);
}

async::DataRequest DataProxy::loadFeatures()
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
    //returns the request
    return createRequest(reply, &DataProxy::parseFeatures);
}

async::DataRequest DataProxy::loadImageAlignment()
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
    //returns the request
    return createRequest(reply, &DataProxy::parseImageAlignment);
}

async::DataRequest DataProxy::loadUser()
{
    //clear container
    m_user.clear();
    //creates the requet
    NetworkCommand *cmd = RESTCommandFactory::getUser(m_configurationManager);
    NetworkReply *reply = m_networkManager->httpRequest(cmd);
    //delete the command
    cmd->deleteLater();
    //returns the request
    return createRequest(reply, &DataProxy::parseUser);
}

async::DataRequest DataProxy::loadGeneSelections()
{
    //clean up the containers
    m_geneSelectionsList.clear();
    //creates the requet
    NetworkCommand* cmd = RESTCommandFactory::getSelections(m_configurationManager);
    NetworkReply *reply = m_networkManager->httpRequest(cmd);
    //delete the command
    cmd->deleteLater();
    //returns the request
    return createRequest(reply, &DataProxy::parseGeneSelections);
}

async::DataRequest DataProxy::updateGeneSelection(GeneSelectionPtr geneSelection)
{
    // intermediary dto object
    GeneSelectionDTO dto(*geneSelection);
    NetworkCommand *cmd =
            RESTCommandFactory::upateSelectionBySelectionId(m_configurationManager,
                                                            geneSelection->id());
    //append json data
    cmd->setBody(dto.toJson());
    NetworkReply *reply = m_networkManager->httpRequest(cmd);
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
    cmd->setBody(dto.toJson());
    NetworkReply *reply = m_networkManager->httpRequest(cmd);
    //delete the command
    cmd->deleteLater();
    //return the request
    return createRequest(reply);
}

async::DataRequest DataProxy::removeSelection(const QString &selectionId)
{
    NetworkCommand *cmd =
            RESTCommandFactory::removeSelectionBySelectionId(m_configurationManager, selectionId);
    NetworkReply *reply = m_networkManager->httpRequest(cmd);
    //delete the command
    cmd->deleteLater();
    //return the request
    return createRequest(reply);
}

async::DataRequest DataProxy::loadCellTissueByName(const QString& name)
{
    //remove image if already exists and add the newly created one
    if (m_cellTissueImages.contains(name)) {
        m_cellTissueImages.remove(name);
    }
    //creates the request
    NetworkCommand *cmd =
            RESTCommandFactory::getCellTissueFigureByName(m_configurationManager, name);
    NetworkReply *reply = m_networkManager->httpRequest(cmd);
    reply->setProperty("figure_name", QVariant::fromValue<QString>(name));
    //delete the command
    cmd->deleteLater();
    return createRequest(reply, &DataProxy::parseCellTissueImage);
}

async::DataRequest DataProxy::loadMinVersion()
{
    NetworkCommand *cmd = RESTCommandFactory::getMinVersion(m_configurationManager);
    // send empty flags to ensure access token is not appended to request
    NetworkReply *reply = m_networkManager->httpRequest(cmd, NetworkManager::Empty);
    //delete the command
    cmd->deleteLater();
    return createRequest(reply, &DataProxy::parseMinVersion);
}

async::DataRequest DataProxy::loadAccessToken(const StringPair &username,
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
    return createRequest(reply, &DataProxy::parseOAuth2);
}

async::DataRequest DataProxy::createRequest(NetworkReply *reply,bool
                                            (DataProxy::*parseFunc)(NetworkReply *reply))
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
    connect(reply, SIGNAL(signalFinished(QVariant)), &loop, SLOT(quit()));
    loop.exec(QEventLoop::ExcludeUserInputEvents
              | QEventLoop::X11ExcludeTimers | QEventLoop::WaitForMoreEvents);


    async::DataRequest request(async::DataRequest::CodeSuccess);

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
                    error(new Error(tr("Data Error"),
                                    tr("There was an error downloading data"), this));
            request.addError(error);
            request.return_code(async::DataRequest::CodeError);
        } else if (returnCode == NetworkReply::CodeAbort) {
            //nothing for now
            request.return_code(async::DataRequest::CodeAbort);
        } else {
            bool parsedOk = true;
            if (parseFunc != nullptr) {
                parsedOk = (this->*parseFunc)(reply);
            }

            //errors could happen parsing the data
            if (reply->hasErrors() || !parsedOk) {
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

//TODO this can be optimized and run concurrently
bool DataProxy::parseFeatures(NetworkReply *reply)
{
    bool dirty = false;

    const QJsonDocument &doc = reply->getJSON();
    if (doc.isNull() || doc.isEmpty()) {
        return false;
    }

    //temp container to assure the uniqueness of the Gene objects
    QHash<QString, GenePtr> geneNameToGene;

    // aux variable to parse feature objects
    FeatureDTO dto;

    // iterate the features from the JSON object
    foreach(QVariant var, doc.toVariant().toList()) {
        // convert from QVariant to FeatureDTO and create Feature object
        data::parseObject(var, &dto);
        FeaturePtr feature = FeaturePtr(new Feature(dto.feature()));

        const QString geneName = feature->gene();

        //create unique gene object
        GenePtr gene;
        if (geneNameToGene.contains(geneName)) {
            gene = geneNameToGene.value(geneName);
        } else {
            gene = GenePtr(new Gene(geneName));
            geneNameToGene.insert(geneName, gene);
            m_genesList.push_back(gene);
        }

        //update feature with the unique gene object
        feature->geneObject(gene);
        //update containers
        m_featuresList.push_back(feature);
        m_geneFeaturesMap.insert(gene->name(), feature);

        dirty = true;
    }

    return dirty;
}

//TODO this can be optimized and run concurrently
bool DataProxy::parseCellTissueImage(NetworkReply *reply)
{
    // get filename and file raw data (check if it was encoded or not)
    const QByteArray &rawImage = reply->getRaw();
    Q_ASSERT(!rawImage.isEmpty() && !rawImage.isNull());

    const QString &imageName = reply->property("figure_name").toString();
    Q_ASSERT(!imageName.isEmpty() && !imageName.isNull());

    //create the image from raw data
    QImage image = QImage::fromData(rawImage);

    const bool imageOk = !image.isNull();
    if (imageOk) {
        m_cellTissueImages.insert(imageName, image);
    }

    return imageOk;
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
