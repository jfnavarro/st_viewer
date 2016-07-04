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
#include "dataModel/Gene.h"
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
    FeaturesHandler(DataProxy::FeatureList &featureList,
                    DataProxy::GeneNameToObject &geneNameToGene)
        : featuresList(featureList)
        , geneNameToGene(geneNameToGene)
    {
    }

    bool Null() { return false; }
    bool Bool(bool) { return false; }

    bool Int(int i)
    {
        varMap.insert(currentKey, i);
        return true;
    }

    bool Uint(int u)
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

    bool RawNumber(const char *, SizeType, bool) { return true; }

    bool String(const char *str, SizeType length, bool)
    {
        Q_UNUSED(length);
        varMap.insert(currentKey, QString(str));
        return true;
    }

    bool StartObject()
    {
        varMap.clear();
        currentKey.clear();
        return true;
    }

    bool Key(const char *str, SizeType length, bool)
    {
        Q_UNUSED(length);
        currentKey = QString(str);
        return true;
    }

    bool EndObject(SizeType)
    {
        // create feature object from variant map
        FeatureDTO dto;
        data::parseObject(varMap, &dto);
        auto feature = std::make_shared<Feature>(dto.feature());
        Q_ASSERT(feature);

        // get the gene name
        const QString gene_name = feature->gene();
        Q_ASSERT(!gene_name.isNull() && !gene_name.isEmpty());

        // create unique gene object
        if (!geneNameToGene.contains(gene_name)) {
            geneNameToGene.insert(gene_name, std::make_shared<Gene>(Gene(gene_name)));
        }

        // update containers
        featuresList.push_back(feature);

        return true;
    }

    bool StartArray() { return true; }
    bool EndArray(SizeType) { return true; }

private:
    DataProxy::FeatureList &featuresList;
    DataProxy::GeneNameToObject &geneNameToGene;
    QString currentKey;
    QVariantMap varMap;
};

DataProxy::DataProxy(QObject *parent)
    : QObject(parent)
    , m_user(nullptr)
    , m_networkManager(nullptr)
{
    m_networkManager.reset(new NetworkManager(this));
    Q_ASSERT(!m_networkManager.isNull());
}

DataProxy::~DataProxy()
{
    clean();
}

void DataProxy::clean()
{
    qDebug() << "Cleaning memory cache in Dataproxy";
    // every data member is a smart pointer
    m_datasetList.clear();
    m_userSelectionList.clear();
    m_imageAlignment.reset();
    m_chip.reset();
    m_featuresList.clear();
    m_cellTissueImages.clear();
    m_minVersion = MinVersionArray();
    m_accessToken = OAuth2TokenDTO();
    m_user.reset();
    m_geneNameToObject.clear();
}

void DataProxy::cleanAll()
{
    qDebug() << "Cleaning memory cache and disk cache in Dataproxy";
    clean();
    m_networkManager->cleanCache();
}

const DataProxy::DatasetList &DataProxy::getDatasetList() const
{
    return m_datasetList;
}

const DataProxy::DatasetPtr DataProxy::getDatasetById(const QString &datasetId) const
{
    // dataset list will always be short so a simple find function is enough
    for (auto &dataset : m_datasetList) {
        if (dataset->id() == datasetId) {
            return dataset;
        }
    }

    return nullptr;
}

const DataProxy::GeneList DataProxy::getGeneList() const
{
    return m_geneNameToObject.values();
}

DataProxy::GenePtr DataProxy::geneGeneObject(const QString &gene_name) const
{
    return m_geneNameToObject.value(gene_name);
}

const DataProxy::FeatureList &DataProxy::getFeatureList() const
{
    return m_featuresList;
}

const DataProxy::UserPtr DataProxy::getUser() const
{
    return m_user;
}

const DataProxy::ChipPtr DataProxy::getChip() const
{
    return m_chip;
}

const DataProxy::ImageAlignmentPtr DataProxy::getImageAlignment() const
{
    return m_imageAlignment;
}

const QByteArray DataProxy::getFigureRed() const
{
    Q_ASSERT(m_imageAlignment && !m_imageAlignment->figureRed().isNull()
             && !m_imageAlignment->figureRed().isEmpty());
    return m_cellTissueImages.value(m_imageAlignment->figureRed());
}

const QByteArray DataProxy::getFigureBlue() const
{
    Q_ASSERT(m_imageAlignment && !m_imageAlignment->figureBlue().isNull()
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

bool DataProxy::loadDatasets()
{
    // clean up container (only datasets downloaded from network)
    m_datasetList.erase(std::remove_if(m_datasetList.begin(),
                                       m_datasetList.end(),
                                       [](DatasetPtr dataset) { return dataset->downloaded(); }),
                        m_datasetList.end());
    // creates the request
    const auto cmd = RESTCommandFactory::getDatasets(m_configurationManager);
    auto reply = m_networkManager->httpRequest(cmd);
    // create the download request (sync)
    const bool status_download = createRequest(reply);
    bool status_parsing = false;
    if (status_download) {
        status_parsing = parseRequest(reply, DataProxy::DatasetsDownloaded);
    }
    // returns status
    return status_download && status_parsing;
}

bool DataProxy::addDataset(const Dataset &dataset)
{
    auto dataset_ptr = std::make_shared<Dataset>(dataset);
    if (dataset_ptr) {
        m_datasetList.push_back(dataset_ptr);
        return true;
    } else {
        return false;
    }
}

bool DataProxy::updateDataset(const Dataset &dataset)
{
    // intermediary dto object
    DatasetDTO dto(dataset);
    auto cmd = RESTCommandFactory::updateDatasetByDatasetId(m_configurationManager, dataset.id());
    // append json data
    cmd->setBody(dto.toJson());
    auto reply = m_networkManager->httpRequest(cmd);
    // create the download request (sync)
    const bool status_download = createRequest(reply);
    bool status_parsing = false;
    if (status_download) {
        qDebug() << "Dataset " << dataset.id() << " edited";
        status_parsing = parseRequest(reply, DataProxy::DatasetModified);
    }
    // returns status
    return status_download && status_parsing;
}

bool DataProxy::removeDataset(const QString &datasetId, const bool is_downloaded)
{
    if (!is_downloaded) {
        // the dataset was imported locally..
        return parseRemoveDataset(datasetId);
    }
    const auto cmd
        = RESTCommandFactory::removeDatasetByDatasetId(m_configurationManager, datasetId);
    auto reply = m_networkManager->httpRequest(cmd);
    // add dataset ID to the reply metaproperty
    reply->setProperty("dataset_id", QVariant::fromValue<QString>(datasetId));
    // create the download request (sync)
    const bool status_download = createRequest(reply);
    bool status_parsing = false;
    if (status_download) {
        status_parsing = parseRequest(reply, DataProxy::DatasetRemoved);
    }
    // returns status
    return status_download && status_parsing;
}

bool DataProxy::loadDatasetContent(const DatasetPtr dataset)
{
    if (!dataset || !m_user) {
        return false;
    }

    // load image alignment
    const bool image_alignment = loadImageAlignment(dataset->imageAlignmentId());
    if (!image_alignment || !m_imageAlignment) {
        qDebug() << "Error downloading image alignment...";
        return false;
    }

    // load cell tissue figure one and two
    const bool image_one = loadCellTissueByName(m_imageAlignment->figureBlue());
    bool image_two = true;
    if (image_one && m_user->hasSpecialRole()) {
        // load cell tissue two (no need to download it for role USER)
        image_two = loadCellTissueByName(m_imageAlignment->figureRed());
    }

    if (!image_one || !image_two) {
        qDebug() << "Error downloading images...";
        return false;
    }

    // load features
    const bool features = loadFeatures(dataset->id());
    if (!features) {
        qDebug() << "Error downloading st data features...";
        return false;
    }

    // load chip
    const bool chip = loadChip(m_imageAlignment->chipId());
    if (!chip) {
        qDebug() << "Error downloading chip...";
        return false;
    }

    return true;
}

bool DataProxy::loadChip(const QString &chipId)
{
    Q_ASSERT(!chipId.isNull() && !chipId.isEmpty());
    // clear container
    m_chip.reset();
    // creates the request
    const auto cmd = RESTCommandFactory::getChipByChipId(m_configurationManager, chipId);
    auto reply = m_networkManager->httpRequest(cmd);
    // create the download request (sync)
    const bool status_download = createRequest(reply);
    bool status_parsing = false;
    if (status_download) {
        status_parsing = parseRequest(reply, DataProxy::ChipDownloaded);
    }
    // returns status
    return status_download && status_parsing;
}

bool DataProxy::loadChip(const Chip &chip)
{
    m_chip = std::make_shared<Chip>(chip);
    return m_chip != nullptr;
}

bool DataProxy::loadFeatures(const QString &datasetId)
{
    Q_ASSERT(!datasetId.isNull() && !datasetId.isEmpty());
    // clear the containers
    m_geneNameToObject.clear();
    m_featuresList.clear();
    // creates the request
    const auto cmd = RESTCommandFactory::getFeatureByDatasetId(m_configurationManager, datasetId);
    auto reply = m_networkManager->httpRequest(cmd);
    // create the download request (sync)
    const bool status_download = createRequest(reply);
    bool status_parsing = false;
    if (status_download) {
        status_parsing = parseRequest(reply, DataProxy::FeaturesDownloaded);
    }
    // returns status
    return status_download && status_parsing;
}

bool DataProxy::loadFeatures(const QByteArray &rawData)
{
    return parseFeatures(rawData);
}

bool DataProxy::loadImageAlignment(const QString &imageAlignmentId)
{
    Q_ASSERT(!imageAlignmentId.isNull() && !imageAlignmentId.isEmpty());
    // clear container
    m_imageAlignment.reset();
    // creates the request
    const auto cmd
        = RESTCommandFactory::getImageAlignmentById(m_configurationManager, imageAlignmentId);
    auto reply = m_networkManager->httpRequest(cmd);
    // create the download request (sync)
    const bool status_download = createRequest(reply);
    bool status_parsing = false;
    if (status_download) {
        status_parsing = parseRequest(reply, DataProxy::ImageAlignmentDownloaded);
    }
    // returns status
    return status_download && status_parsing;
}

bool DataProxy::loadImageAlignment(const ImageAlignment &alignment)
{
    m_imageAlignment = std::make_shared<ImageAlignment>(alignment);
    return m_imageAlignment != nullptr;
}

bool DataProxy::loadUser()
{
    // clear container
    m_user.reset();
    // creates the requet
    const auto cmd = RESTCommandFactory::getUser(m_configurationManager);
    auto reply = m_networkManager->httpRequest(cmd);
    // create the download request (sync)
    const bool status_download = createRequest(reply);
    bool status_parsing = false;
    if (status_download) {
        status_parsing = parseRequest(reply, DataProxy::UserDownloaded);
    }
    // returns status
    return status_download && status_parsing;
}

bool DataProxy::loadUserSelections()
{
    // clean up currently download selections
    m_userSelectionList.erase(std::remove_if(m_userSelectionList.begin(),
                                             m_userSelectionList.end(),
                                             [](UserSelectionPtr selection) {
                                  return selection->saved();
                              }),
                              m_userSelectionList.end());
    // creates the requet
    const auto cmd = RESTCommandFactory::getSelections(m_configurationManager);
    auto reply = m_networkManager->httpRequest(cmd);
    // create the download request (sync)
    const bool status_download = createRequest(reply);
    bool status_parsing = false;
    if (status_download) {
        status_parsing = parseRequest(reply, DataProxy::UserSelectionsDownloaded);
    }
    // returns status
    return status_download && status_parsing;
}

bool DataProxy::updateUserSelection(const UserSelection &userSelection)
{
    // intermediary dto object
    UserSelectionDTO dto(userSelection);
    auto cmd = RESTCommandFactory::upateSelectionBySelectionId(m_configurationManager,
                                                               userSelection.id());
    // append json data
    cmd->setBody(dto.toJson());
    auto reply = m_networkManager->httpRequest(cmd);
    // create the download request (sync)
    const bool status_download = createRequest(reply);
    bool status_parsing = false;
    if (status_download) {
        status_parsing = parseRequest(reply, DataProxy::UserSelectionModified);
    }
    // returns status
    return status_download && status_parsing;
}

bool DataProxy::addUserSelection(const UserSelection &userSelection, bool save)
{
    if (!save) {
        auto new_selection = std::make_shared<UserSelection>(userSelection);
        if (new_selection) {
            m_userSelectionList.push_back(new_selection);
            return true;
        } else {
            return false;
        }
    }
    // intermediary dto object
    UserSelectionDTO dto(userSelection);
    auto cmd = RESTCommandFactory::addSelection(m_configurationManager);
    // append json data
    const QByteArray &body = dto.toJson();
    cmd->setBody(body);
    auto reply = m_networkManager->httpRequest(cmd);
    // create the download request (sync)
    const bool status_download = createRequest(reply);
    bool status_parsing = false;
    if (status_download) {
        status_parsing = parseRequest(reply, DataProxy::UserSelectionModified);
    }
    // returns status
    return status_download && status_parsing;
}

bool DataProxy::removeSelection(const QString &selectionId, const bool is_downloaded)
{
    if (!is_downloaded) {
        return parseRemoveUserSelection(selectionId);
    }
    const auto cmd
        = RESTCommandFactory::removeSelectionBySelectionId(m_configurationManager, selectionId);
    auto reply = m_networkManager->httpRequest(cmd);
    // add the selection id to the reply
    reply->setProperty("selection_id", QVariant::fromValue<QString>(selectionId));
    // create the download request (sync)
    const bool status_download = createRequest(reply);
    bool status_parsing = false;
    if (status_download) {
        status_parsing = parseRequest(reply, DataProxy::UserSelectionRemoved);
    }
    // returns status
    return status_download && status_parsing;
}

bool DataProxy::loadCellTissueByName(const QString &name)
{
    // remove image if already exists and add the newly created one
    m_cellTissueImages.remove(name);

    // creates the request
    const auto cmd = RESTCommandFactory::getCellTissueFigureByName(m_configurationManager, name);
    auto reply = m_networkManager->httpRequest(cmd);
    // add figure name to reply metaproperty
    reply->setProperty("figure_name", QVariant::fromValue<QString>(name));
    // create the download request (sync)
    const bool status_download = createRequest(reply);
    bool status_parsing = false;
    if (status_download) {
        status_parsing = parseRequest(reply, DataProxy::TissueImageDownloaded);
    }
    // returns status
    return status_download && status_parsing;
}

bool DataProxy::loadCellTissueImage(const QByteArray &rawData, const QString &imageName)
{
    return parseCellTissueImage(rawData, imageName);
}

bool DataProxy::loadMinVersion()
{
    const auto cmd = RESTCommandFactory::getMinVersion(m_configurationManager);
    // send empty flags to ensure access token is not appended to request
    auto reply = m_networkManager->httpRequest(cmd, NetworkManager::Empty);
    // create the download request (sync)
    const bool status_download = createRequest(reply);
    bool status_parsing = false;
    if (status_download) {
        status_parsing = parseRequest(reply, DataProxy::MinVersionDownloaded);
    }
    // returns status
    return status_download && status_parsing;
}

bool DataProxy::loadAccessToken(const QPair<QString, QString> &username,
                                const QPair<QString, QString> &password)
{
    auto cmd = RESTCommandFactory::getAuthorizationToken(m_configurationManager);
    // add username and password to the request
    cmd->addQueryItem(username.first, username.second);
    cmd->addQueryItem(password.first, password.second);
    // send empty flags to ensure access token is not appended to request
    auto reply = m_networkManager->httpRequest(cmd, NetworkManager::Empty);
    // create the download request (sync)
    const bool status_download = createRequest(reply);
    bool status_parsing = false;
    if (status_download) {
        status_parsing = parseRequest(reply, DataProxy::AccessTokenDownloaded);
    }
    // returns status
    return status_download && status_parsing;
}

bool DataProxy::createRequest(QSharedPointer<NetworkReply> reply)
{
    if (reply == nullptr) {
        qDebug() << "[DataProxy] : Error, the NetworkReply is null,"
                    "there must have been a network error";
        QWidget *mainWidget = QApplication::desktop()->screen();
        QMessageBox::critical(mainWidget,
                              tr("Error downloading data"),
                              tr("There was probably a network problem."));
        return false;
    }

    QEventLoop loop;
    connect(reply.data(), SIGNAL(signalFinished(QVariant)), &loop, SLOT(quit()));
    loop.exec();

    if (reply == nullptr) {
        QWidget *mainWidget = QApplication::desktop()->screen();
        QMessageBox::critical(mainWidget,
                              tr("Error downloading data"),
                              tr("There was probably a network problem."));
        return false;
    }

    if (!reply->hasErrors()) {
        const NetworkReply::ReturnCode returnCode
            = static_cast<NetworkReply::ReturnCode>(reply->return_code());

        if (returnCode == NetworkReply::CodeError) {
            // strange..the reply does not have registered errors but yet
            // the returned code is ERROR
            QWidget *mainWidget = QApplication::desktop()->screen();
            QMessageBox::critical(mainWidget,
                                  tr("Error downloading data"),
                                  tr("There was probably a network problem."));
            return false;
        }

    } else {
        const auto error = reply->parseErrors();
        QWidget *mainWidget = QApplication::desktop()->screen();
        QMessageBox::critical(mainWidget, error->name(), error->description());
        return false;
    }

    return true;
}

bool DataProxy::userLogIn() const
{
    return m_user && m_user->enabled();
}

bool DataProxy::parseRequest(QSharedPointer<NetworkReply> reply, const DownloadType &type)
{
    Q_ASSERT(reply);
    bool parsedOk = false;
    switch (type) {
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
        parsedOk = parseCellTissueImage(reply->getRaw(), reply->property("figure_name").toString());
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
    case UserSelectionModified:
    case DatasetModified:
        parsedOk = true;
        break;
    default:
        break;
    }

    // errors could happen parsing the data
    if (reply->hasErrors() || !parsedOk) {
        const auto error = reply->parseErrors();
        const QString error_name = error.isNull() ? tr("Error parsing data") : error->name();
        const QString error_dcr =
                error.isNull() ?
                    tr("There was an error parsing the data object from the remote server")
                  : error->description();
        QWidget *mainWidget = QApplication::desktop()->screen();
        QMessageBox::critical(mainWidget, error_name, error_dcr);
        return false;
    } else {
        qDebug() << "Data from network request parsed correctly...";
        return true;
    }
}

// TODO this function is causing the CPU to freeze with big datasets
// solution is to run it concurrently
bool DataProxy::parseFeatures(const QByteArray &rawData)
{
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    FeaturesHandler handler(m_featuresList, m_geneNameToObject);
    Reader reader;
    StringStream is(rawData.data());
    const bool parsedOk = reader.Parse(is, handler);
    QGuiApplication::restoreOverrideCursor();
    return parsedOk;
}

bool DataProxy::parseCellTissueImage(const QByteArray &rawData, const QString &imageName)
{
    // check data and filename
    const bool parsedOk = !rawData.isEmpty() && !rawData.isNull() && !imageName.isEmpty()
                          && !imageName.isNull();
    if (parsedOk) {
        // store the image as raw data
        m_cellTissueImages.insert(imageName, rawData);
    }

    return parsedOk;
}

bool DataProxy::parseDatasets(const QJsonDocument &doc)
{
    if (doc.isNull() || doc.isEmpty()) {
        // an error is added to the reply when this happens
        return false;
    }

    // intermediary parse object
    DatasetDTO dto;

    // parse the objects
    for (const QVariant &var : doc.toVariant().toList()) {
        data::parseObject(var, &dto);
        DatasetPtr dataset = std::make_shared<Dataset>(dto.dataset());
        Q_ASSERT(dataset);
        // the commented lines are to only adds the datasets that the user has access to
        //const auto granted_users = dataset->grantedAccounts();
        //if (std::find(granted_users.begin(), granted_users.end(), m_user->id())
        //    != granted_users.end()) {
        //    m_datasetList.push_back(dataset);
        //}
        m_datasetList.push_back(dataset);
    }

    return true;
}

bool DataProxy::parseRemoveDataset(const QString &datasetId)
{
    // remove dataset from list
    m_datasetList.erase(std::remove_if(m_datasetList.begin(),
                                       m_datasetList.end(),
                                       [=](DatasetPtr dataset) {
                            return dataset->id() == datasetId;
                        }),
                        m_datasetList.end());

    // remove selections created on that dataset and that are not saved in the cloud
    // the selections of the dataset that are stored in the cloud will automatically be deleted
    m_userSelectionList.erase(std::remove_if(m_userSelectionList.begin(),
                                             m_userSelectionList.end(),
                                             [=](UserSelectionPtr selection) {
                                  return selection->datasetId() == datasetId && !selection->saved();
                              }),
                              m_userSelectionList.end());
    // TODO should check that it was removed
    return true;
}

bool DataProxy::parseUserSelections(const QJsonDocument &doc)
{
    if (doc.isNull() || doc.isEmpty()) {
        // an error is present in reply when this happens
        return false;
    }

    // intermediary parse object
    UserSelectionDTO dto;

    // parse the data
    for (const QVariant &var : doc.toVariant().toList()) {
        data::parseObject(var, &dto);
        UserSelectionPtr selection = std::make_shared<UserSelection>(dto.userSelection());
        Q_ASSERT(selection);
        // get the dataset name from the cached datasets
        // TODO if we enter the user selections view without having downloaded the
        // datasets the dataset name cannot be fetched
        const auto dataset = getDatasetById(selection->datasetId());
        if (!dataset) {
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

bool DataProxy::parseRemoveUserSelection(const QString &selectionId)
{
    // remove element from the list
    m_userSelectionList.erase(std::remove_if(m_userSelectionList.begin(),
                                             m_userSelectionList.end(),
                                             [=](UserSelectionPtr selection) {
                                  return selection->id() == selectionId;
                              }),
                              m_userSelectionList.end());
    // TODO should check that it was removed
    return true;
}

bool DataProxy::parseUser(const QJsonDocument &doc)
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
    m_user = std::make_shared<User>(dto.user());
    return m_user != nullptr;
}

bool DataProxy::parseImageAlignment(const QJsonDocument &doc)
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
    m_imageAlignment = std::make_shared<ImageAlignment>(dto.imageAlignment());
    return m_imageAlignment != nullptr;
}

bool DataProxy::parseChip(const QJsonDocument &doc)
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
    m_chip = std::make_shared<Chip>(dto.chip());
    return m_chip != nullptr;
}

bool DataProxy::parseMinVersion(const QJsonDocument &doc)
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
    return m_minVersion.size() == 3;
}

bool DataProxy::parseOAuth2(const QJsonDocument &doc)
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
    return !m_accessToken.accessToken().isNull() && !m_accessToken.accessToken().isEmpty();
}
