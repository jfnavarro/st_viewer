#include "DataProxy.h"

#include <QDebug>
#include <QEventLoop>
#include <QJsonDocument>
#include <QObject>
#include <QtGlobal>
#include <QImage>
#include <QMessageBox>
#include <QImageReader>
#include <QApplication>
#include <QDesktopWidget>
#include <QUuid>

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
#include "dataModel/UserSelectionDTO.h"
#include "dataModel/LastModifiedDTO.h"
#include "dataModel/MinVersionDTO.h"
#include "dataModel/UserSelection.h"
#include "dataModel/Chip.h"
#include "dataModel/Dataset.h"
#include "dataModel/Feature.h"
#include "dataModel/Gene.h"
#include "dataModel/ImageAlignment.h"
#include "dataModel/User.h"
#include "rapidjson/reader.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/error/en.h"
#include <iostream>
#include <sstream>

using namespace rapidjson;

// Handler with call backs for rapidjson
// explicitly made to parse the Features JSON type object
// references to the containers are passed and will be filled up
// with the parsed objects
// TODO move to a separate file
struct FeaturesHandler {

public:
    FeaturesHandler(DataProxy::FeatureList& m_featuresList, DataProxy::GeneList& m_genesList)
        : featuresList(m_featuresList)
        , genesList(m_genesList)
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
        // create feature object from variant map
        FeatureDTO dto;
        data::parseObject(varMap, &dto);
        DataProxy::FeaturePtr feature = DataProxy::FeaturePtr(new Feature(dto.feature()));

        // get the gene name
        const QString gene = feature->gene();

        // create unique gene object
        DataProxy::GenePtr genePtr;
        if (geneNameToGene.contains(gene)) {
            genePtr = geneNameToGene.value(gene);
        } else {
            genePtr = DataProxy::GenePtr(new Gene(gene));
            geneNameToGene.insert(gene, genePtr);
            genesList.push_back(genePtr);
        }

        // update feature with the unique gene object
        feature->geneObject(genePtr);

        // update containers
        featuresList.push_back(feature);

        return true;
    }

    bool StartArray() { return true; }
    bool EndArray(SizeType) { return true; }

private:
    DataProxy::FeatureList& featuresList;
    DataProxy::GeneList& genesList;
    QHash<QString, DataProxy::GenePtr> geneNameToGene;
    QString currentKey;
    QVariantMap varMap;
};

DataProxy::DataProxy(QObject* parent)
    : QObject(parent)
    , m_user(nullptr)
    , m_networkManager(nullptr)
    , m_activeDownloads(0)
{
    // initialize data containers
    m_user = UserPtr(new User());

    m_networkManager = new NetworkManager(this);
    Q_ASSERT(!m_networkManager.isNull());
}

DataProxy::~DataProxy()
{
    m_user.clear();
    clean();
}

void DataProxy::clean()
{
    qDebug() << "Cleaning memory cache in Dataproxy";
    // every data member is a smart pointer
    m_datasetList.clear();
    m_userSelectionList.clear();
    m_imageAlignment.clear();
    m_chip.clear();
    m_featuresList.clear();
    m_genesList.clear();
    m_cellTissueImages.clear();
    m_minVersion = MinVersionArray();
    m_accessToken = OAuth2TokenDTO();
    m_activeDownloads = 0;
    m_activeNetworkReplies.clear();
    m_user.clear();
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
    // dataset list will always be short so a simple find function is enough
    foreach (DataProxy::DatasetPtr dataset, m_datasetList) {
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
    Q_ASSERT(!m_imageAlignment.isNull() && !m_imageAlignment->figureRed().isNull()
             && !m_imageAlignment->figureRed().isEmpty());
    return m_cellTissueImages.value(m_imageAlignment->figureRed());
}

const QByteArray DataProxy::getFigureBlue() const
{
    Q_ASSERT(!m_imageAlignment.isNull() && !m_imageAlignment->figureBlue().isNull()
             && !m_imageAlignment->figureBlue().isEmpty());
    return m_cellTissueImages.value(m_imageAlignment->figureBlue());
}

const DataProxy::UserSelectionList DataProxy::getUserSelectionList() const
{
    return m_userSelectionList;
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
    // clean up container (only online)
    m_datasetList.erase(std::remove_if(m_datasetList.begin(), m_datasetList.end(),
                                       [](DatasetPtr dataset) {return dataset->downloaded();}),
                        m_datasetList.end());
    // creates the request
    NetworkCommand* cmd = RESTCommandFactory::getDatasets(m_configurationManager);
    NetworkReply* reply = m_networkManager->httpRequest(cmd);
    // delete the command
    cmd->deleteLater();
    // create the download request
    createRequest(reply, DataProxy::DatasetsDownloaded);
}

void DataProxy::addDataset(const Dataset& dataset)
{
    m_datasetList.push_back(DatasetPtr(new Dataset(dataset)));
}

void DataProxy::updateDataset(const Dataset& dataset)
{
    // intermediary dto object
    DatasetDTO dto(dataset);
    NetworkCommand* cmd
        = RESTCommandFactory::updateDatasetByDatasetId(m_configurationManager, dataset.id());
    // append json data
    cmd->setBody(dto.toJson());
    NetworkReply* reply = m_networkManager->httpRequest(cmd);
    // delete the command
    cmd->deleteLater();
    // create the download request
    createRequest(reply, DataProxy::DatasetModified);
}

void DataProxy::removeDataset(const QString& datasetId)
{
    NetworkCommand* cmd
        = RESTCommandFactory::removeDatasetByDatasetId(m_configurationManager, datasetId);
    NetworkReply* reply = m_networkManager->httpRequest(cmd);
    // add dataset ID to the reply metaproperty
    reply->setProperty("dataset_id", QVariant::fromValue<QString>(datasetId));
    // delete the command
    cmd->deleteLater();
    // create the download request
    createRequest(reply, DataProxy::DatasetRemoved);
}

void DataProxy::loadDatasetContent(const QString& datasetId)
{
    Q_ASSERT(!datasetId.isNull() && !datasetId.isEmpty());
    Q_ASSERT(!m_imageAlignment.isNull());

    // load cell tissue blue
    loadCellTissueByName(m_imageAlignment->figureBlue());
    Q_ASSERT(!m_user.isNull());
    if (m_user->hasSpecialRole()) {
        // load cell tissue red (no need to download for role USER)
        loadCellTissueByName(m_imageAlignment->figureRed());
    }

    // load features
    loadFeatures(datasetId);

    // load chip
    loadChip();
}

void DataProxy::loadChip()
{
    Q_ASSERT(!m_imageAlignment.isNull() && !m_imageAlignment->chipId().isNull()
             && !m_imageAlignment->chipId().isEmpty());
    // clear container
    m_chip.clear();
    // creates the request
    NetworkCommand* cmd
        = RESTCommandFactory::getChipByChipId(m_configurationManager, m_imageAlignment->chipId());
    NetworkReply* reply = m_networkManager->httpRequest(cmd);
    // delete the command
    cmd->deleteLater();
    // create the download request
    createRequest(reply, DataProxy::ChipDownloaded);
}

void DataProxy::loadChip(const Chip& chip)
{
    m_chip = ChipPtr(new Chip(chip));
}

void DataProxy::loadFeatures(const QString& datasetId)
{
    Q_ASSERT(!datasetId.isNull() && !datasetId.isEmpty());
    // clear the containers
    m_genesList.clear();
    m_featuresList.clear();
    // creates the request
    NetworkCommand* cmd =
            RESTCommandFactory::getFeatureByDatasetId(m_configurationManager, datasetId);
    NetworkReply* reply = m_networkManager->httpRequest(cmd);
    // delete the command
    cmd->deleteLater();
    // create the download request
    createRequest(reply, DataProxy::FeaturesDownloaded);
}

void DataProxy::loadImageAlignment(const QString& imageAlignmentId)
{
    Q_ASSERT(!imageAlignmentId.isNull() && !imageAlignmentId.isEmpty());
    // clear container
    m_imageAlignment.clear();
    // creates the request
    NetworkCommand* cmd
        = RESTCommandFactory::getImageAlignmentById(m_configurationManager, imageAlignmentId);
    NetworkReply* reply = m_networkManager->httpRequest(cmd);
    // delete the command
    cmd->deleteLater();
    // create the download request
    createRequest(reply, DataProxy::ImageAlignmentDownloaded);
}

void DataProxy::loadImageAlignment(const ImageAlignment& alignment)
{
    ImageAlignmentPtr imageAlignement = ImageAlignmentPtr(new ImageAlignment(alignment));
    m_imageAlignment = imageAlignement;
}

void DataProxy::loadUser()
{
    // clear container
    m_user.clear();
    // creates the requet
    NetworkCommand* cmd = RESTCommandFactory::getUser(m_configurationManager);
    NetworkReply* reply = m_networkManager->httpRequest(cmd);
    // delete the command
    cmd->deleteLater();
    // create the download request
    createRequest(reply, DataProxy::UserDownloaded);
}

void DataProxy::loadUserSelections()
{
    // clean up currently download selections
    m_userSelectionList.erase(std::remove_if(m_userSelectionList.begin(), m_userSelectionList.end(),
                                             [](UserSelectionPtr selection) {return selection->saved();}),
            m_userSelectionList.end());
    // creates the requet
    NetworkCommand* cmd = RESTCommandFactory::getSelections(m_configurationManager);
    NetworkReply* reply = m_networkManager->httpRequest(cmd);
    // delete the command
    cmd->deleteLater();
    // create the download request
    createRequest(reply, DataProxy::UserSelectionsDownloaded);
}

void DataProxy::updateUserSelection(const UserSelection& userSelection)
{
    // intermediary dto object
    UserSelectionDTO dto(userSelection);
    NetworkCommand* cmd = RESTCommandFactory::upateSelectionBySelectionId(m_configurationManager,
                                                                          userSelection.id());
    // append json data
    cmd->setBody(dto.toJson());
    NetworkReply* reply = m_networkManager->httpRequest(cmd);
    // delete the command
    cmd->deleteLater();
    // create the download request
    createRequest(reply, DataProxy::UserSelectionModified);
}

void DataProxy::addUserSelection(const UserSelection& userSelection, bool save)
{
    if (!save) {
        m_userSelectionList.append(UserSelectionPtr(new UserSelection(userSelection)));
        return;
    }
    // intermediary dto object
    UserSelectionDTO dto(userSelection);
    NetworkCommand* cmd = RESTCommandFactory::addSelection(m_configurationManager);
    // append json data
    const QByteArray& body = dto.toJson();
    cmd->setBody(body);
    NetworkReply* reply = m_networkManager->httpRequest(cmd);
    // delete the command
    cmd->deleteLater();
    // create the download request
    createRequest(reply, DataProxy::UserSelectionModified);
}

void DataProxy::removeSelection(const QString& selectionId)
{
    NetworkCommand* cmd
        = RESTCommandFactory::removeSelectionBySelectionId(m_configurationManager, selectionId);
    NetworkReply* reply = m_networkManager->httpRequest(cmd);
    // add the selection id to the reply
    reply->setProperty("selection_id", QVariant::fromValue<QString>(selectionId));
    // delete the command
    cmd->deleteLater();
    // create the download request
    createRequest(reply, DataProxy::UserSelectionRemoved);
}

void DataProxy::loadCellTissueByName(const QString& name)
{
    // remove image if already exists and add the newly created one
    if (m_cellTissueImages.contains(name)) {
        m_cellTissueImages.remove(name);
    }
    // creates the request
    NetworkCommand* cmd
        = RESTCommandFactory::getCellTissueFigureByName(m_configurationManager, name);
    NetworkReply* reply = m_networkManager->httpRequest(cmd);
    // add figure name to reply metaproperty
    reply->setProperty("figure_name", QVariant::fromValue<QString>(name));
    // delete the command
    cmd->deleteLater();
    // create the download request
    createRequest(reply, DataProxy::TissueImageDownloaded);
}

void DataProxy::loadMinVersion()
{
    NetworkCommand* cmd = RESTCommandFactory::getMinVersion(m_configurationManager);
    // send empty flags to ensure access token is not appended to request
    NetworkReply* reply = m_networkManager->httpRequest(cmd, NetworkManager::Empty);
    // delete the command
    cmd->deleteLater();
    // create the download request
    createRequest(reply, DataProxy::MinVersionDownloaded);
}

void DataProxy::loadAccessToken(const StringPair& username, const StringPair& password)
{
    NetworkCommand* cmd = RESTCommandFactory::getAuthorizationToken(m_configurationManager);
    // add username and password to the request
    cmd->addQueryItem(username.first, username.second);
    cmd->addQueryItem(password.first, password.second);
    // send empty flags to ensure access token is not appended to request
    NetworkReply* reply = m_networkManager->httpRequest(cmd, NetworkManager::Empty);
    // delete the command
    cmd->deleteLater();
    // create the download request
    createRequest(reply, DataProxy::AccessTokenDownloaded);
}

void DataProxy::createRequest(NetworkReply* reply, DataProxy::DownloadType type)
{
    if (reply == nullptr) {
        qDebug() << "[DataProxy] : Error, the NetworkReply is null,"
                    "there must have been a network error";
        QWidget* mainWidget = QApplication::desktop()->screen();
        QMessageBox::critical(mainWidget,
                              tr("Error downloading data"),
                              tr("There was probably a network problem."));
        //TODO should send a signal here to notify that something went wrong
    } else {
        // increase active downloads counter
        m_activeDownloads++;
        // add reply to the active download replies
        m_activeNetworkReplies.insert(reply, type);
    }
}

void DataProxy::activateCurrentDownloads() const
{
    foreach (NetworkReply* reply, m_activeNetworkReplies.keys()) {
        // connect parsing function to reply
        connect(reply, SIGNAL(signalFinished(QVariant)), this, SLOT(slotProcessDownload()));
    }
}

bool DataProxy::userLogIn() const
{
    return !m_user.isNull() && m_user->enabled();
}

unsigned DataProxy::getActiveDownloads() const
{
    return m_activeDownloads;
}

void DataProxy::slotProcessDownload()
{
    // first decrease the counter of currently active downloads
    if (m_activeDownloads > 0) {
        m_activeDownloads--;
    } else {
        qDebug() << "A network reply call back has been invoked with no active downloads";
    }

    // get reply object from the caller
    NetworkReply* reply = qobject_cast<NetworkReply*>(sender());
    if (reply == nullptr) {
        QWidget* mainWidget = QApplication::desktop()->screen();
        QMessageBox::critical(mainWidget,
                              tr("Error downloading data"),
                              tr("There was probably a network problem."));
        return;
    }

    // get the reply type
    const DataProxy::DownloadType type = m_activeNetworkReplies.value(reply);
    // status failed by default
    DataProxy::DownloadStatus status = DataProxy::Failed;

    // remove reply from the container
    const bool removedOK = m_activeNetworkReplies.remove(reply);
    if (!removedOK) {
        qDebug() << "A network reply call back has been invoked but it has been processed already";
    }

    if (!reply->hasErrors()) {
        const NetworkReply::ReturnCode returnCode
            = static_cast<NetworkReply::ReturnCode>(reply->return_code());

        if (returnCode == NetworkReply::CodeError) {
            // strange..the reply does not have registered errors but yet
            // the returned code is ERROR
            QWidget* mainWidget = QApplication::desktop()->screen();
            QMessageBox::critical(mainWidget,
                                  tr("Error downloading data"),
                                  tr("There was probably a network problem."));
        } else if (returnCode == NetworkReply::CodeAbort) {
            status = DataProxy::Aborted;
        } else {
            bool parsedOk = true;
            switch(type) {
            case MinVersionDownloaded:
                parsedOk = parseMinVersion(reply->getJSON());
                break;
            case AccessTokenDownloaded:
                parsedOk = parseOAuth2(reply->getJSON());
                break;
            case UserDownloaded:
                parsedOk = parseUser(reply->getJSON());
                break;
            case DatasetsDownloaded:
                parsedOk = parseDatasets(reply->getJSON());
                break;
            case DatasetRemoved:
                parsedOk = parseRemoveDataset(reply->property("dataset_id").toString());
                break;
            case ImageAlignmentDownloaded:
                parsedOk = parseImageAlignment(reply->getJSON());
                break;
            case ChipDownloaded:
                parsedOk = parseChip(reply->getJSON());
                break;
            case TissueImageDownloaded:
                parsedOk = parseCellTissueImage(reply->getRaw(),
                                                reply->property("figure_name").toString());
                break;
            case FeaturesDownloaded:
                parsedOk = parseFeatures(reply->getRaw());
                break;
            case UserSelectionsDownloaded:
                parsedOk = parseUserSelections(reply->getJSON());
                break;
            case UserSelectionRemoved:
                parsedOk = parseRemoveUserSelection(reply->property("selection_id").toString());
                break;
            default:
                break;
            }

            // errors could happen parsing the data
            if (reply->hasErrors() || !parsedOk) {
                const auto error = reply->parseErrors();
                Q_ASSERT(!error.isNull());
                QWidget* mainWidget = QApplication::desktop()->screen();
                QMessageBox::critical(mainWidget, error->name(), error->description());
            } else {
                status = DataProxy::Success;
            }
        }
    } else {
        const auto error = reply->parseErrors();
        QWidget* mainWidget = QApplication::desktop()->screen();
        QMessageBox::critical(mainWidget, error->name(), error->description());
    }

    // delete reply
    reply->deleteLater();

    // check if it is the last download if so emit the signal to notify
    if (m_activeDownloads == 0) {
        switch(type) {
        case MinVersionDownloaded:
            emit signalMinVersionDownloaded(status);
            break;
        case AccessTokenDownloaded:
            emit signalAcessTokenDownloaded(status);
            break;
        case UserDownloaded:
            emit signalUserDownloaded(status);
            break;
        case DatasetsDownloaded:
            emit signalDatasetsDownloaded(status);
            break;
        case DatasetRemoved:
            emit signalDatasetRemoved(status);
            break;
        case ImageAlignmentDownloaded:
            emit signalImageAlignmentDownloaded(status);
            break;
        case ChipDownloaded:
            emit signalChipDownloaded(status);
            break;
        case TissueImageDownloaded:
            emit signalTissueImageDownloaded(status);
            break;
        case FeaturesDownloaded:
            emit signalFeaturesDownloaded(status);
            break;
        case UserSelectionsDownloaded:
            emit signalUserSelectionsDownloaded(status);
            break;
        case UserSelectionAdded:
        case UserSelectionModified:
            emit signalUserSelectionModified(status);
            break;
        case UserSelectionRemoved:
            emit signalUserSelectionDeleted(status);
            break;
        case DatasetModified:
            emit signalDatasetModified(status);
            break;
        default:
            break;
        }
    }
}

void DataProxy::slotAbortActiveDownloads()
{
    foreach (NetworkReply* reply, m_activeNetworkReplies.keys()) {
        // abort will trigger the process signal which will to the function above
        // being called, perhaps change that logic
        reply->slotAbort();
    }
}

// TODO this function is causing the CPU to freeze with big datasets
// solution is to run it concurrently but that implies changes
// in the main logic of this class
bool DataProxy::parseFeatures(const QByteArray& rawData)
{
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    FeaturesHandler handler(m_featuresList, m_genesList);
    Reader reader;
    StringStream is(rawData.data());
    const bool parsedOk = reader.Parse(is, handler);
    QGuiApplication::restoreOverrideCursor();
    return parsedOk;
}

bool DataProxy::parseCellTissueImage(const QByteArray& rawData, const QString& imageName)
{
    // check data and filename
    const bool parsedOk = !rawData.isEmpty() && !rawData.isNull()
            && !imageName.isEmpty() && !imageName.isNull();
    if (parsedOk) {
        // store the image as raw data
        m_cellTissueImages.insert(imageName, rawData);
    }

    return parsedOk;
}

bool DataProxy::parseDatasets(const QJsonDocument& doc)
{
    if (doc.isNull() || doc.isEmpty()) {
        // an error is added to the reply when this happens
        return false;
    }

    // intermediary parse object
    DatasetDTO dto;

    // parse the objects
    foreach (QVariant var, doc.toVariant().toList()) {
        data::parseObject(var, &dto);
        DatasetPtr dataset = DatasetPtr(new Dataset(dto.dataset()));
        m_datasetList.push_back(dataset);
    }

    return true;
}

bool DataProxy::parseRemoveDataset(const QString& datasetId)
{
    // remove and return true of it was found and removed
    return std::remove_if(m_datasetList.begin(), m_datasetList.end(),
                          [=](DatasetPtr dataset)
    {return dataset->id() == datasetId;}) == m_datasetList.end();
}

bool DataProxy::parseUserSelections(const QJsonDocument& doc)
{
    if (doc.isNull() || doc.isEmpty()) {
        // an error is present in reply when this happens
        return false;
    }

    // intermediary parse object
    UserSelectionDTO dto;

    // parse the data
    foreach (QVariant var, doc.toVariant().toList()) {
        data::parseObject(var, &dto);
        UserSelectionPtr selection = UserSelectionPtr(new UserSelection(dto.geneSelection()));
        // get the dataset name from the cached datasets
        // TODO if we enter the user selections view without having downloaded the
        // datasets the dataset name cannot be fetched
        const DatasetPtr dataset = getDatasetById(selection->datasetId());
        if (dataset.isNull()) {
            selection->datasetName(tr("Unknown dataset"));
        } else {
            selection->datasetName(dataset->name());
        }
        // update the enable field
        selection->saved(true);
        // store the selection
        m_userSelectionList.push_back(selection);
    }

    return true;
}

bool DataProxy::parseRemoveUserSelection(const QString& selectionId)
{
    // remove and return true of it was found and removed
    return std::remove_if(m_userSelectionList.begin(), m_userSelectionList.end(),
                          [=](UserSelectionPtr selection)
    {return selection->id() == selectionId;}) == m_userSelectionList.end();
}

bool DataProxy::parseUser(const QJsonDocument& doc)
{
    if (doc.isNull() || doc.isEmpty()) {
        // an error is present in reply when this happens
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

bool DataProxy::parseImageAlignment(const QJsonDocument& doc)
{
    if (doc.isNull() || doc.isEmpty()) {
        // an error is present in reply when this happen
        return false;
    }

    // intermediary parse object
    ImageAlignmentDTO dto;

    // image alignment should only contain one object
    const QVariant root = doc.toVariant();
    data::parseObject(root, &dto);
    ImageAlignmentPtr imageAlignement = ImageAlignmentPtr(new ImageAlignment(dto.imageAlignment()));
    m_imageAlignment = imageAlignement;
    return true;
}

bool DataProxy::parseChip(const QJsonDocument& doc)
{
    if (doc.isNull() || doc.isEmpty()) {
        // an error is present in reply when this happens
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

bool DataProxy::parseMinVersion(const QJsonDocument& doc)
{
    if (doc.isNull() || doc.isEmpty()) {
        // an error is present in reply when this happens
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

bool DataProxy::parseOAuth2(const QJsonDocument& doc)
{
    if (doc.isNull() || doc.isEmpty()) {
        // an error is present in reply when this happens
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
