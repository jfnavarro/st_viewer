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

#include "utils/Singleton.h"

// data objects
#include "dataModel/Chip.h"
#include "dataModel/Dataset.h"
#include "dataModel/Feature.h"
#include "dataModel/Gene.h"
#include "dataModel/ImageAlignment.h"
#include "dataModel/User.h"
#include "dataModel/GeneSelection.h"

#include "data/DataStore.h"

// network
#include "network/NetworkManager.h"
#include "network/NetworkReply.h"

namespace async {
class DataRequest;
}

// DataProxy is a globally accessible all-in-all data store. It provides an
// interface to access remotely stored data and a means of storing and managing
// the transferred data locally.
// It provides the backbone for most of the data models which are in turn used
// to access specific subsets of the data store in the data proxy.
// it also provides and API to load the data from the network
class DataProxy : public Singleton<DataProxy>
{
    Q_ENUMS(DataType)

public:

    enum DataType {
        TissueDataType,
        ChipDataType,
        DatasetDataType,
        FeatureDataType,
        GeneDataType,
        ImageAlignmentDataType,
        GeneSelectionDataType,
        UserType
    };

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
    //   - get gene from feature ID or Gene Name
    //   - get features from dataset ID
    //   - get genes from dataset ID

    //list of unique genes
    typedef QVector<GenePtr> GeneList;
    //map of unique genes (gene name to gene pointer)
    typedef QMap<QString, GenePtr> GeneMap;
    //gene list hashed by dataset id
    typedef QMap<QString, GeneList> GeneListMap;
    //gene map hashed by dataset id
    typedef QMap<QString, GeneMap> GeneMapMap;
    //list of features
    typedef QVector<FeaturePtr> FeatureList;
    // map of features hashed by feature id
    typedef QMap<QString, FeaturePtr> FeatureMap;
    //features hashed by dataset id
    typedef QMap<QString, FeatureList> FeatureListMap;
    //features hashed by dataset id and feature id
    typedef QMap<QString, FeatureMap> FeatureMapMap;
    //features hashed by dataset id and gene name
    typedef QMap<QString, FeatureListMap> FeatureListGeneMap;
    //chip hashed by dataset id
    typedef QMap<QString, ChipPtr> ChipMap;
    //list of unique datasets
    typedef QList<DatasetPtr> DatasetList;
    //datasets hashed by dataset id
    typedef QMap<QString, DatasetPtr> DatasetMap;
    //image alignment hashed by image alignment id
    typedef QMap<QString, ImageAlignmentPtr> ImageAlignmentMap;
    //gene selection objects
    typedef QList<GeneSelectionPtr> GeneSelectionList;
    //gene selection hashed by id
    typedef QMap<QString, GeneSelectionPtr> GeneSelectionMap;
    //cell figure hashed by figure name (figure names are unique)
    typedef QMap<QString, QString> CellFigureMap;

    DataProxy();
    ~DataProxy();

    //init instance
    void init();
    //finalize instance
    void finalize();
    //clean up memory cache
    void clean();
    //clean up memory cache and local cache
    void cleanAll();

    //data loaders
    // chip
    async::DataRequest loadChipById(const QString& chipId);
    // datasets
    async::DataRequest loadDatasets();
    // features
    async::DataRequest loadFeatureByDatasetId(const QString& datasetId);
    // genes
    async::DataRequest loadGenesByDatasetId(const QString& datasetId);
    // image alignment
    async::DataRequest loadImageAlignmentById(const QString& imageAlignmentId);
    // cell tissue figure
    async::DataRequest loadCellTissueByName(const QString& name);
    // current logged user
    async::DataRequest loadUser();
    // selection objects
    async::DataRequest loadGeneSelections();

    //data getters
    const DatasetList getDatasetList() const;
    GeneList& getGeneList(const QString& datasetId);
    FeatureList& getFeatureList(const QString& datasetId);
    GenePtr getGene(const QString& datasetId, const QString& geneName);
    FeatureList& getGeneFeatureList(const QString& datasetId,
                                      const QString& geneName);
    DatasetPtr getDatasetById(const QString& datasetId) const;
    ImageAlignmentPtr getImageAlignment(const QString& imageAlignmentId);
    ChipPtr getChip(const QString& chipId);
    UserPtr getUser() const;
    DataStore::resourceDeviceType getFigure(const QString& figureId);
    const GeneSelectionList getGeneSelections() const;
    const QString getSelectedDataset() const;

    //setters
    void setSelectedDataset(const QString &datasetId) const;

    //updaters
    //async::DataRequest updateDataset(const Dataset& dataset);
    //async::DataRequest updateUser(const User& user);

    //creation
    async::DataRequest addGeneSelection(const GeneSelection& geneSelection);

    //deletion
    async::DataRequest removeGeneSelectionById(const QString& id);

private:

    bool hasCellTissue(const QString& name) const;
    bool hasImageAlignment(const QString& datasetId) const;
    bool hasGene(const QString& datasetId) const;
    bool hasFeature(const QString& datasetId) const;
    bool hasFeature(const QString& datasetId, const QString& gene) const;
    bool hasDatasets() const;
    bool hasDataset(const QString& datasetId) const;
    bool hasChip(const QString& chipId) const;

    // internal functions to parse network reply
    bool parseData(NetworkReply* reply, const QVariantMap& map);

    //internal function to create network requests
    async::DataRequest createRequest(NetworkReply *reply);

    GeneMap& getGeneMap(const QString &datasetId);
    FeatureMap& getFeatureMap(const QString &datasetId);

    // available datasets mapped by dataset id
    DatasetMap m_datasetMap;
    // present genes mapped by dataset Id
    GeneListMap m_geneListMap;
    // present gene maps hashed by dataset id
    GeneMapMap m_geneMap;
    // chip arrays mapped by dataset id
    ChipMap m_chipMap;
    // feature mapped by dataset id and feature id
    FeatureMapMap m_featureMap;
    // map dataset id to list of features
    FeatureListMap m_featureListMap;
    // map dataset id and gene name to list of features
    FeatureListGeneMap m_geneFeatureListMap;
    // the current user logged in
    UserPtr m_user;
     // map dataset id to image alignment
    ImageAlignmentMap m_imageAlignmentMap;
    // the current gene selections
    GeneSelectionMap m_geneSelectionsMap;
    // the current selected dataset
    mutable QString m_selected_datasetId;

    //network manager to make network requests
    //must be a member variable
    NetworkManager m_networkManager;

    //data storage to handle image files
    DataStore m_dataStore;

    Q_DISABLE_COPY(DataProxy)
};

#endif  /* DATAPROXY_H */
