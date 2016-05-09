#ifndef DATAPROXY_H
#define DATAPROXY_H

#include <QObject>
#include <QVector>
#include <QMap>
#include <QSharedPointer>
#include "config/Configuration.h"
#include "dataModel/OAuth2TokenDTO.h"

#include <array>
#include <unordered_map>

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

namespace std
{
template <>
struct hash<QString> {
    size_t operator()(const QString &s) const { return qHash(s); }
};
}

// DataProxy is a globally accessible all-in-all data store. It provides an
// interface to access remotely stored data and means of storing and managing
// the transferred data locally.
// It provides the backbone for most of the data models which are in turn used
// to access specific subsets of the data store in the data proxy.
// it also provides and API to load the data from the network
class DataProxy : public QObject
{
    Q_OBJECT
    Q_ENUMS(DownloadType)

public:
    enum DownloadType {
        None, // do nothing, useful to remove requests
        MinVersionDownloaded,
        AccessTokenDownloaded,
        UserDownloaded,
        DatasetsDownloaded,
        DatasetModified,
        DatasetRemoved,
        ImageAlignmentDownloaded,
        ChipDownloaded,
        TissueImageDownloaded,
        FeaturesDownloaded,
        UserSelectionsDownloaded,
        UserSelectionModified,
        UserSelectionRemoved,
        UserSelectionAdded
    };

    // MAIN CONTAINERS (MVC)
    typedef std::shared_ptr<Chip> ChipPtr;
    typedef std::shared_ptr<Dataset> DatasetPtr;
    typedef std::shared_ptr<Feature> FeaturePtr;
    typedef std::shared_ptr<Gene> GenePtr;
    typedef std::shared_ptr<ImageAlignment> ImageAlignmentPtr;
    typedef std::shared_ptr<UserSelection> UserSelectionPtr;
    typedef std::shared_ptr<User> UserPtr;

    // TODO find a way to update or notify DataProxy when data is updated in the
    // backend (database)

    // TODO not really needed to use QSharedPointer(they are expensive), it would
    // be better to use
    // direct references or other type of smart pointer

    // TODO separate data API and data adquisition

    // TODO replace JSON for binary format for the features (data frame or tab
    // delimited)

    // TODO make the parsing of the features data asynchronous

    // TODO consider to compute the features rendering data here

    // TODO Currently dataProxy does not support caching. The dataset content
    // variables are unique for the dataset currently opened. We should cache
    // the dataset content variable by dataset ID

    // list of unique genes
    typedef std::vector<GenePtr> GeneList;
    // list of features
    typedef std::vector<FeaturePtr> FeatureList;
    // list of unique datasets
    typedef std::vector<DatasetPtr> DatasetList;
    // list of user selections
    typedef std::vector<UserSelectionPtr> UserSelectionList;
    // cell figure hashed by figure name (figure names are unique)
    typedef std::unordered_map<QString, QByteArray> CellFigureMap;
    // array of three elements containing the min version supported
    typedef std::array<qulonglong, 3> MinVersionArray;
    // gene name to gene object
    typedef std::unordered_map<QString, GenePtr> GeneNameToObject;

    explicit DataProxy(QObject *parent = 0);
    ~DataProxy();

    // clean up memory cache
    void clean();
    // clean up memory cache and local cache (hard drive)
    void cleanAll();

    // DATA LOADERS
    // The data loaders methods will make a network request to download
    // the data from the network (performing integrity checks)
    // The downloading will be made synchronous.

    // Downloads and parses the user's datasets from the database
    // Before it will remove previously downloaded datasets
    // Returns true if the download and parsing went fine
    bool loadDatasets();
    // Downloads and parses the dataset content (chip, cell images and features)
    // from the database
    // Returns true if the download and parsing went fine
    bool loadDatasetContent(const DatasetPtr dataset);
    // Downloads and parses the current logged user from the database
    // Returns true if the download and parsing went fine
    bool loadUser();
    // Download and parses the selections made by the user from the database
    // Beofre it will remove previously downloaded selections
    // Returns true if the download and parsing went fine
    bool loadUserSelections();
    // Downloads and parses the min supported version from the database
    // Returns true if the download and parsing went fine
    bool loadMinVersion();
    // Download and parses OAuth2 access token for the user credentials given
    // Returns true if the download and parsing went fine
    bool loadAccessToken(const std::pair<QString, QString> &username,
                         const std::pair<QString, QString> &password);
    // Download the chip and parses it
    // Returns true if the download and parsing went fine
    bool loadChip(const QString &chipId);
    // Download and parses the ST Data of a dataset from the database
    // Returns true if the download and parsing went fine
    bool loadFeatures(const QString &datasetId);
    // Download and parses an alignment from the database
    // Returns true if the download and parsing went fine
    bool loadImageAlignment(const QString &imageAlignmentId);
    // Downloads and parses a cell tissue figure from the database
    // Returns true if the download and parsing went fine
    bool loadCellTissueByName(const QString &name);

    // DATA LOADERS LOCALLY
    // Methods to add data locally to the containers, for instance
    // data imported by the user

    // chip imported locally
    // returns true if the parsing was correct
    bool loadChip(const Chip &chip);
    // image alignment imported locally
    // returns true if the parsing was correct
    bool loadImageAlignment(const ImageAlignment &alignment);
    // st data features imported locally from file
    // returns true if the parsing was correct
    bool loadFeatures(const QByteArray &rawData);
    // cell tissue image imported from file
    // returns true if the parsing was correct
    bool loadCellTissueImage(const QByteArray &rawData, const QString &imageName);

    // DATA UPDATERS
    // Data updaters are meant to be used to update an object in the database
    // The object must be passed as argument and it has to have a method to
    // to represent a JSON object.

    // dataset
    // Returns true if the download and parsing went fine
    bool updateDataset(const Dataset &dataset);
    // user
    // Returns true if the download and parsing went fine
    bool updateUser(const User &user);
    // gene selection
    // Returns true if the download and parsing went fine
    bool updateUserSelection(const UserSelection &geneSelection);

    // DATA CREATION
    // Data creation methods are meant to create a new object in the database
    // or to add locally imported objects to the containers

    // add an user selection object to the DB
    // save = True if the object needs to be saved in the database
    // Returns true if the download and parsing went fine
    bool addUserSelection(const UserSelection &userSelection, const bool save = false);

    // add a dataset to the list of dataset locally (not in the network)
    // TODO add support to save datasets to the database
    bool addDataset(const Dataset &dataset);

    // DATA DELETION
    // data deletion methods are meant to remove an object from the database

    // dataset
    // is_downloaded true if the dataset was downloaded from the database
    // Returns true if the download and parsing went fine
    bool removeDataset(const QString &datasetId, const bool is_downloaded = true);
    // gene selection
    // is_downloaded true if the selection was downloaded from the database
    // Returns true if the download and parsing went fine
    bool removeSelection(const QString &selectionId, const bool is_downloaded = true);

    // API DATA GETTERS
    // to retrieve the download objects (caller must have downloaded the object
    // previously)
    // DataProxy assumes only one dataset can be open at the time

    // returns the list of currently loaded datasets
    const DatasetList &getDatasetList() const;
    // returns the dataset with the same id as the given one (returns null if not
    // found)
    const DatasetPtr getDatasetById(const QString &datasetId) const;

    // returns the list of currently loaded genes
    // a current dataset object must be selected otherwise it returns an empty
    // list
    const GeneList getGeneList() const;

    // returns the gene object of the given gene name
    GenePtr geneGeneObject(const QString &gene_name) const;

    // returns the list of currently loaded features
    // a current dataset object must be selected otherwise it returns an empty
    // list
    const FeatureList &getFeatureList() const;

    // returns the list of currently loaded features whose gene name matches
    // geneName
    // a current dataset object must be selected otherwise it returns an empty
    // list
    const FeatureList getGeneFeatureList(const QString &geneName) const;

    // returns the currently loaded image alignment object
    // a current dataset object must be selected otherwise it returns a null
    // object
    ImageAlignmentPtr getImageAlignment() const;

    // returns the currently loaded chip
    // a current dataset object must be selected otherwise it returns a null
    // object
    ChipPtr getChip() const;

    // a current dataset object must be selected otherwise it returns a null
    // object
    UserPtr getUser() const;

    // returns the currently loaded image blue or red as a byte array
    // a current dataset object must be selected otherwise it returns an empty
    // image
    const QByteArray getFigureRed() const;
    const QByteArray getFigureBlue() const;

    // returns the currently loaded list of genes selections
    const UserSelectionList getUserSelectionList() const;

    // returns the minimum supported version of the software in the backend
    const MinVersionArray getMinVersion() const;

    // returns the current access token if any
    const OAuth2TokenDTO getAccessToken() const;

    // true if the user is currently logged in
    bool userLogIn() const;

public slots:

private slots:

signals:

private:
    // Internal function to create network requests for data objects
    // The network call will be synchrnous and the function will
    // return true of the the network call was successful (no errors)
    // or false otherwise.
    bool createRequest(QSharedPointer<NetworkReply> reply);
    // Once the data of a network request has been downloaded the request
    // can be parsed to extract the data with this method
    // Returns true if the datas was parsed correctly false otherwise
    bool parseRequest(QSharedPointer<NetworkReply> reply, const DownloadType &type);

    // PARSING FUNCTIONS
    // Functions to parse the data downloaded from the network

    // function to parse all the features and genes and add them to the containers
    // returns true if the parsing was correct
    bool parseFeatures(const QByteArray &rawData);

    // function to parse a cell tissue image and add it to the container
    // returns true if the parsing was correct
    bool parseCellTissueImage(const QByteArray &rawData, const QString &imageName);

    // function to parse the datasets and add them to the container
    // returns true if the parsing was correct
    bool parseDatasets(const QJsonDocument &doc);

    // removes the dataset from the container and its associated selections
    // returns true if it was removed
    bool parseRemoveDataset(const QString &datasetId);

    // function to parse the user selections and add them to the container
    // returns true if the parsing was correct
    bool parseUserSelections(const QJsonDocument &doc);

    // removes the selection from the local container
    // returns true of it was removed
    bool parseRemoveUserSelection(const QString &selectionId);

    // function to parse the User and added to the container
    // returns true if the parsing was correct
    bool parseUser(const QJsonDocument &doc);

    // function to parse the image alignment object and added to the container
    // returns true if the parsing was correct
    bool parseImageAlignment(const QJsonDocument &doc);

    // function to parse a chip and added to the container
    // returns true if the parsing was correct
    bool parseChip(const QJsonDocument &doc);

    // function to parse the min version supported and added to the container
    // returns true if the parsing was correct
    bool parseMinVersion(const QJsonDocument &doc);

    // function to parse the OAuth2 access token and added to the container
    // returns true if the parsing was correct
    bool parseOAuth2(const QJsonDocument &doc);

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
    // the map of gene names to gene objects
    GeneNameToObject m_geneNameToObject;
    // the current images (blue and red) for the selected dataset
    CellFigureMap m_cellTissueImages;
    // the application min supported version
    MinVersionArray m_minVersion;
    // the Access token object
    OAuth2TokenDTO m_accessToken;
    // configuration manager instance
    Configuration m_configurationManager;
    // network manager to make network requests (dataproxy owns it)
    QPointer<NetworkManager> m_networkManager;

    Q_DISABLE_COPY(DataProxy)
};

#endif /* DATAPROXY_H */
