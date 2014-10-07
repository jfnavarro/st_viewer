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

#include "dataModel/Chip.h"
#include "dataModel/Dataset.h"
#include "dataModel/Feature.h"
#include "dataModel/Gene.h"
#include "dataModel/ImageAlignment.h"
#include "dataModel/User.h"
#include "dataModel/GeneSelection.h"
#include "dataModel/MinVersionDTO.h"

#include "config/Configuration.h"

namespace async {
class DataRequest;
}
class WrappedFileDTO;
class DatasetDTO;
class GeneSelectionDTO;
class UserDTO;
class ChipDTO;
class ImangeAlignmentDTO;
class QImage;
class NetworkManager;
class AuthorizationManager;
class NetworkReply;

// DataProxy is a globally accessible all-in-all data store. It provides an
// interface to access remotely stored data and a means of storing and managing
// the transferred data locally.
// It provides the backbone for most of the data models which are in turn used
// to access specific subsets of the data store in the data proxy.
// it also provides and API to load the data from the network
class DataProxy : public QObject
{
    Q_OBJECT

public:
    // MAIN CONTAINERS (MVC)
    typedef QSharedPointer<Chip> ChipPtr;
    typedef QSharedPointer<Dataset> DatasetPtr;
    typedef QSharedPointer<Feature> FeaturePtr;
    typedef QSharedPointer<Gene> GenePtr;
    typedef QSharedPointer<ImageAlignment> ImageAlignmentPtr;
    typedef QSharedPointer<GeneSelection> GeneSelectionPtr;
    typedef QSharedPointer<User> UserPtr;

    //TODO number of containers can be decreased for Genes and Features
    //what is really needed is a super fast lookup for :
    //   - get features from dataset ID and Gene Name
    //   - get gene/s from feature ID or Gene Name (replace feature-gene text field for gene ptr)
    //   - get features from dataset ID
    //   - get genes from dataset ID
    //   - get gene from dataset ID and gene Name

    //TODO some of the QMap could be replaced for QHash(std::unordered_map) which
    //is faster

    //TODO replace loading everything into memory for using file system cache, serialization
    //will save memmory overhead

    //TODO too much logic in one class :
    //split data adquisition and data loading

    //TODO data adquision could splitted for each entity and
    //use the observer pattern to account for entities that are connected

    //TODO find a way to update DataProxy when data is updated trough the backend (a timed request)

    //TODO not really needed to use QSharedPointer(they are expensive), it would be better to use
    //direct references or other type of smart pointer

    //list of unique genes
    typedef QList<GenePtr> GeneList;
    //multi-map of unique genes (gene name to feature objects)
    typedef QMultiHash<QString, FeaturePtr> GeneFeatureMap;

    //list of features
    typedef QList<FeaturePtr> FeatureList;

    //list of unique datasets
    typedef QList<DatasetPtr> DatasetList;

    //gene selection objects
    typedef QList<GeneSelectionPtr> GeneSelectionList;

    //cell figure hashed by figure name (figure names are unique)
    typedef QHash<QString, QImage> CellFigureMap;

    //array of three elements containing the min version supported
    typedef std::array<qulonglong, 3> MinVersionArray;

    DataProxy(QObject *parent = 0);
    ~DataProxy();

    //clean up memory cache
    void clean();
    //clean up memory cache and local cache (hard drive)
    void cleanAll();

    //returns the authorization manager which is owned by dataProxy
    //TODO DataProxy should not own AuthorizationManager, perhaps
    //move the ownership to stVi
    QPointer<AuthorizationManager> getAuthorizationManager() const;

    //DATA LOADERS
    //data loaders are meant to be used to load data from the network.
    //they return an object that contains the status of the operation
    //and the errors if any.
    //DataRequest can be connected to signals and it has ABORT functionality

    // datasets
    async::DataRequest loadDatasets();
    // dataset content (chip, image alignment, cell images and features)
    async::DataRequest loadDatasetContent();
    // current logged user
    async::DataRequest loadUser();
    // selection objects
    async::DataRequest loadGeneSelections();
    // min version supported
    async::DataRequest loadMinVersion();

    //DATA UPDATERS

    // dataset
    async::DataRequest updateDataset(DatasetPtr dataset);
    // user
    async::DataRequest updateUser(const User& user);
    // gene selection
    async::DataRequest updateGeneSelection(GeneSelectionPtr geneSelection);

    //DATA CREATION

    // gene selection
    async::DataRequest addGeneSelection(const GeneSelection& geneSelection);

    // DATA DELETION

    // dataset
    async::DataRequest removeDataset(const QString& datasetId);
    // gene selection
    async::DataRequest removeSelection(const QString& selectionId);

    // API DATA GETTERS

    // returns the list of currently loaded datasets
    const DatasetList& getDatasetList() const;
    // returns the dataset with the same id as the given one (returns null if not found)
    const DatasetPtr getDatasetById(const QString &datasetId) const;

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

    // returns the currently loaded image blue or red
    // a current dataset object must be selected otherwise it returns an empty image
    const QImage getFigureRed() const;
    const QImage getFigureBlue() const;

    // returns the currently loaded list of genes selections
    const GeneSelectionList getGenesSelectionsList() const;

    // returns the minimum supported version of the software in the backend
    const MinVersionArray getMinVersion() const;

    // CURRENT DATASET (state selector)

    // set the currently opened dataset
    // this will define the status of the DataProxy
    void setSelectedDataset(const DatasetPtr dataset) const;
    const DatasetPtr getSelectedDataset() const;
    void resetSelectedDataset();

private:

    // chip (image alignment must has been downloaded first)
    async::DataRequest loadChip();
    // features (dataset must has been downloaded first)
    async::DataRequest loadFeatures();
    // image alignment (dataset must has been downloaded first)
    async::DataRequest loadImageAlignment();
    // cell tissue figure (image alignment must has been downloaded first)
    async::DataRequest loadCellTissueByName(const QString& name);

    //internal function to parse all the features and genes.
    //returns true if the parsing was correct
    bool parseFeatures(const QJsonDocument &doc);

    //internal function to parse a cell tissue image
    //returns true if the parsing was correct
    bool parseCellTissueImage(const QJsonDocument &doc);

    //internal function to parse the datasets
    //returns true if the parsing was correct
    bool parseDatasets(const QJsonDocument &doc);

    //internal function to parse the genes selections
    //returns true if the parsing was correct
    bool parseGeneSelections(const QJsonDocument &doc);

    //internal function to parse the User
    //returns true if the parsing was correct
    bool parseUser(const QJsonDocument &doc);

    //internal function to parse the image alignment object
    //returns true if the parsing was correct
    bool parseImageAlignment(const QJsonDocument &doc);

    //internal function to parse the chip
    //returns true if the parsing was correct
    bool parseChip(const QJsonDocument &doc);

    //internal function to parse the min version supported
    //returns true if the parsing was correct
    bool parseMinVersion(const QJsonDocument &doc);

    //bool parseOAuth2(const QJsonDocument &doc);

    //internal function to create network requests for data objects
    //data will be parsed with the function given as argument if given
    async::DataRequest createRequest(NetworkReply *reply,
                                     bool (DataProxy::*parseFunc)(const QJsonDocument &doc) = nullptr);

    // currently available datasets
    DatasetList m_datasetList;
    // the current user logged in
    UserPtr m_user;
    // the current gene selections for the selected dataset
    GeneSelectionList m_geneSelectionsList;
    // the current alignment object for the selected dataset
    ImageAlignmentPtr m_imageAlignment;
    // the current chip object for the selected dataset
    ChipPtr m_chip;
    // the current features for the selected dataset
    FeatureList m_featuresList;
    // the current unique genes from the features of the selected dataset
    GeneList m_genesList;
    // the current features mapped by gene name
    GeneFeatureMap m_geneFeaturesMap;
    // the current images (blue and red) for the selected dataset
    CellFigureMap m_cellTissueImages;
    // the application min supported version
    MinVersionArray m_minVersion;
    // the current selected dataset
    mutable DatasetPtr m_selectedDataset;

    //configuration manager (dataproxy owns it)
    Configuration m_configurationManager;
    //network manager to make network requests (dataproxy owns it)
    QPointer<NetworkManager> m_networkManager;
    //authorization manager to handle access token (own by DataProxy but altered by InitPage)
    QPointer<AuthorizationManager> m_authorizationManager;

    Q_DISABLE_COPY(DataProxy);
};

#endif  /* DATAPROXY_H */
