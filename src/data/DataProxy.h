/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef DATAPROXY_H
#define DATAPROXY_H

#include <QObject>
#include <QVector>
#include <QMap>
#include <QSharedPointer>
#include "config/Configuration.h"
#include "dataModel/OAuth2TokenDTO.h"
#include <array>

class QByteArray;
class NetworkManager;
class AuthorizationManager;
class NetworkReply;
class UserSelection;
class User;
class ImageAlignment;
class Gene;
class Feature;
class Dataset;
class Chip;
class MinVersionDTO;

// DataProxy is a globally accessible all-in-all data store. It provides an
// interface to access remotely stored data and means of storing and managing
// the transferred data locally.
// It provides the backbone for most of the data models which are in turn used
// to access specific subsets of the data store in the data proxy.
// it also provides and API to load the data from the network
class DataProxy : public QObject
{
    Q_OBJECT
    Q_ENUMS(DownloadStatus)
    Q_ENUMS(DownloadType)

public:
    enum DownloadStatus { Success, Aborted, Failed };

    enum DownloadType {
        None, // do nothing, useful for remove requests
        MinVersionDownloaded,
        AccessTokenDownloaded,
        UserDownloaded,
        DatasetsDownloaded,
        DatasetModified,
        ImageAlignmentDownloaded,
        ChipDownloaded,
        TissueImageDownloaded,
        FeaturesDownloaded,
        UserSelectionsDownloaded,
        UserSelectionModified
    };

    // MAIN CONTAINERS (MVC)
    typedef QSharedPointer<Chip> ChipPtr;
    typedef QSharedPointer<Dataset> DatasetPtr;
    typedef QSharedPointer<Feature> FeaturePtr;
    typedef QSharedPointer<Gene> GenePtr;
    typedef QSharedPointer<ImageAlignment> ImageAlignmentPtr;
    typedef QSharedPointer<UserSelection> UserSelectionPtr;
    typedef QSharedPointer<User> UserPtr;

    // TODO find a way to update DataProxy when data is updated in the backend
    //(a timed request or a listener thread)

    // TODO not really needed to use QSharedPointer(they are expensive), it would be better to use
    // direct references or other type of smart pointer

    // TODO separate data API and data adquisition

    // TODO replace JSON for binary format for the features

    // TODO make the parsing of the data asynchronous

    // TODO find a better make to notify guys using DataProxy when an object has been downloaded

    // TODO study different ways of representing the features (maybe a sparse matrix)

    // TODO consider to compute the features rendering data here

    // TODO Currently dataProxy does not support caching. The dataset content
    // variables are unique for the dataset currently openned. We should cache
    // the dataset content variable by dataset ID

    // list of unique genes
    typedef QList<GenePtr> GeneList;
    // multi-map of unique genes (gene name to feature objects)
    typedef QMultiHash<QString, FeaturePtr> GeneFeatureMap;
    // list of features
    typedef QList<FeaturePtr> FeatureList;
    // list of unique datasets
    typedef QList<DatasetPtr> DatasetList;
    // list of user selections
    typedef QList<UserSelectionPtr> UserSelectionList;
    // cell figure hashed by figure name (figure names are unique)
    typedef QHash<QString, QByteArray> CellFigureMap;
    // array of three elements containing the min version supported
    typedef std::array<qulonglong, 3> MinVersionArray;
    // to pass key-value parameters to loadAccessToken()
    typedef QPair<QString, QString> StringPair;

    explicit DataProxy(QObject* parent = 0);
    ~DataProxy();

    // clean up memory cache
    void clean();
    // clean up memory cache and local cache (hard drive)
    void cleanAll();

    // DATA LOADERS
    // (the method activateCurrentDownloads must be called after invoking any loading)
    // data loaders are meant to be used to load data from the network.
    // Callers are expected to wait for a signal to notify when the data is downloaded,
    // the signal will contain the status of the operation

    // datasets
    void loadDatasets();
    // dataset content (chip, cell images and features)
    // this function assumes the dataset and its image alignment objects are downloaded
    void loadDatasetContent();
    // current logged user
    void loadUser();
    // selection objects (the new user selections will be added to the container)
    void loadUserSelections();
    // min version supported
    void loadMinVersion();
    // OAuth2 access token
    void loadAccessToken(const StringPair& username, const StringPair& password);
    // chip (image alignment must has been downloaded first)
    void loadChip();
    // features (dataset must has been downloaded first)
    void loadFeatures();
    // image alignment (dataset must has been downloaded first)
    void loadImageAlignment();
    // cell tissue figure (image alignment must have been downloaded first)
    void loadCellTissueByName(const QString& name);

    // DATA UPDATERS
    // data updaters are meant to be used to update an object in the database
    // Callers are expected to wait for a signal to notify when the data is downloaded,
    // the signal will contain the status of the operation

    // dataset
    void updateDataset(const Dataset& dataset);
    // user
    void updateUser(const User& user);
    // gene selection
    void updateUserSelection(const UserSelection& geneSelection);

    // DATA CREATION
    // data creation methods are meant to create a new object in the database
    // Callers are expected to wait for a signal to notify when the data is downloaded,
    // the signal will contain the status of the operation

    // add an user selection object to the DB
    void addUserSelection(const UserSelection& userSelection);
    // add an user selection ot the data container of selections (locally not network)
    void addUserSelectionLocal(const UserSelection& userSelection);

    // DATA DELETION
    // data deletion methods are meant to remove an object from the database
    // Callers are expected to wait for a signal to notify when the data is downloaded,
    // the signal will contain the status of the operation

    // dataset
    void removeDataset(const QString& datasetId);
    // gene selection
    void removeSelection(const QString& selectionId);

    // API DATA GETTERS
    // to retrieve the download objects (caller must have downloaded the object previously)
    // TODO add some error control here
    // DataProxy assumes only one dataset can be open at the time

    // returns the list of currently loaded datasets
    const DatasetList& getDatasetList() const;
    // returns the dataset with the same id as the given one (returns null if not found)
    const DatasetPtr getDatasetById(const QString& datasetId) const;

    // returns the list of currently loaded genes
    // a current dataset object must be selected otherwise it returns an empty list
    const GeneList& getGeneList() const;

    // returns the list of currently loaded features
    // a current dataset object must be selected otherwise it returns an empty list
    const FeatureList& getFeatureList() const;

    // returns the list of currently loaded features whose gene name matches geneName
    // a current dataset object must be selected otherwise it returns an empty list
    const FeatureList getGeneFeatureList(const QString& geneName) const;

    // returns the currently loaded image alignment object
    // a current dataset object must be selected otherwise it returns a null object
    ImageAlignmentPtr getImageAlignment() const;

    // returns the currently loaded chip
    // a current dataset object must be selected otherwise it returns a null object
    ChipPtr getChip() const;

    // a current dataset object must be selected otherwise it returns a null object
    UserPtr getUser() const;

    // returns the currently loaded image blue or red as a byte array
    // a current dataset object must be selected otherwise it returns an empty image
    const QByteArray getFigureRed() const;
    const QByteArray getFigureBlue() const;

    // returns the currently loaded list of genes selections
    const UserSelectionList getUserSelectionList() const;

    // returns the minimum supported version of the software in the backend
    const MinVersionArray getMinVersion() const;

    // returns the current access token if any
    const OAuth2TokenDTO getAccessToken() const;

    // CURRENT DATASET (state selector)
    // set the currently opened dataset
    // this will define the status of the DataProxy
    void setSelectedDataset(const DatasetPtr dataset) const;
    const DatasetPtr getSelectedDataset() const;
    void resetSelectedDataset();

    // CURRENTLY ACTIVE DOWNLOADS
    unsigned getActiveDownloads() const;

    // Function to connect the active network requests
    // to the slotProcessDownload. This will enable to process the downloads
    // otherwise the downloads will never be processed. This helps to queue
    // several downloads and process them when necessary
    void activateCurrentDownloads() const;

    // true if the user is currently logged in
    bool userLogIn() const;

public slots:

    // Abort all the current active downloads if any
    void slotAbortActiveDownloads();

private slots:

    // Internal slot to process the call back of a network request
    // which will invoke the callback function linked to the network reply
    // the call back function will load the data and some sanity check
    // will be performed
    void slotProcessDownload();

signals:

    // signal to notify guys using dataProxy about download/s finished
    void signalDownloadFinished(DataProxy::DownloadStatus status, DataProxy::DownloadType type);

private:
    // internal function to parse all the features and genes.
    // returns true if the parsing was correct
    bool parseFeatures(NetworkReply* reply);

    // internal function to parse a cell tissue image
    // returns true if the parsing was correct
    bool parseCellTissueImage(NetworkReply* reply);

    // internal function to parse the datasets
    // returns true if the parsing was correct
    bool parseDatasets(NetworkReply* reply);

    // internal function to make sure to clear the selected dataset
    // in case we remove the dataset that is selected
    bool parseRemoveDataset(NetworkReply* reply);

    // internal function to parse the user selections
    // returns true if the parsing was correct
    bool parseUserSelections(NetworkReply* reply);

    // internal function to parse the User
    // returns true if the parsing was correct
    bool parseUser(NetworkReply* reply);

    // internal function to parse the image alignment object
    // returns true if the parsing was correct
    bool parseImageAlignment(NetworkReply* reply);

    // internal function to parse the chip
    // returns true if the parsing was correct
    bool parseChip(NetworkReply* reply);

    // internal function to parse the min version supported
    // returns true if the parsing was correct
    bool parseMinVersion(NetworkReply* reply);

    // internal function to parse the OAuth2 access token
    // returns true if the parsing was correct
    bool parseOAuth2(NetworkReply* reply);

    // internal function to create network requests for data objects
    // data will be parsed with the function given as argument if given
    // a signal to emit when something goes wrong is also optionally given
    void createRequest(NetworkReply* reply,
                       DataProxy::DownloadType type = None,
                       bool (DataProxy::*parseFunc)(NetworkReply* reply) = nullptr);

    // currently available datasets
    DatasetList m_datasetList;
    // the current user logged in
    UserPtr m_user;
    // the current user selections for the selected dataset
    UserSelectionList m_userSelectionList;
    // the current alignment object for the selected dataset
    ImageAlignmentPtr m_imageAlignment;
    // the current chip object for the selected dataset
    ChipPtr m_chip;
    // the current features for the selected dataset
    FeatureList m_featuresList;
    // the current unique genes from the features of the selected dataset
    GeneList m_genesList;
    // the current images (blue and red) for the selected dataset
    CellFigureMap m_cellTissueImages;
    // the application min supported version
    MinVersionArray m_minVersion;
    // the Access token object
    OAuth2TokenDTO m_accessToken;
    // the current selected dataset
    mutable DatasetPtr m_selectedDataset;

    // configuration manager instance
    Configuration m_configurationManager;
    // network manager to make network requests (dataproxy owns it)
    QPointer<NetworkManager> m_networkManager;

    // to keep track of the current downloads (async)
    unsigned m_activeDownloads;
    // this map represents a reply -> pair(download type, call back function to process the
    // download)
    QHash<NetworkReply*, QPair<DataProxy::DownloadType, bool (DataProxy::*)(NetworkReply* reply)>>
        m_activeNetworkReplies;

    Q_DISABLE_COPY(DataProxy)
};

#endif /* DATAPROXY_H */
