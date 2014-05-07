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
#include "dataModel/FeatureExtended.h"
#include "dataModel/Gene.h"
#include "dataModel/DatasetStatistics.h"
#include "dataModel/User.h"
#include "dataModel/UserExperiment.h"
#include "dataModel/GeneSelection.h"

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
        HitCountDataType,
        UserExperimentDataType,
        UserType
    };

    // MAIN CONTAINERS (MVC)
    typedef QSharedPointer<Chip> ChipPtr;
    typedef QSharedPointer<Dataset> DatasetPtr;
    typedef QSharedPointer<FeatureExtended> FeaturePtr;
    typedef QSharedPointer<Gene> GenePtr;
    typedef QSharedPointer<DatasetStatistics> DatasetStatisticsPtr;
    typedef QSharedPointer<UserExperiment> UserExperimentPtr;
    typedef QSharedPointer<User> UserPtr;

    //list of unique genes
    typedef QVector<GenePtr> GeneList;
    //map of unique genes
    typedef QMap<QString, GenePtr> GeneMap;
    //gene map hashed by dataset id
    typedef QMap<QString, GeneMap> GeneMapMap;
    //gene list hashed by dataset id
    typedef QMap<QString, GeneList> GeneListMap;
    //list of features
    typedef QVector<FeaturePtr> FeatureList;
    // map of features give feature id
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
    typedef QVector<DatasetPtr> DatasetList;
    //datasets hashed by dataset id
    typedef QMap<QString, DatasetPtr> DatasetMap;
    //hitcount hashed by dataset id
    typedef QMap<QString, DatasetStatisticsPtr> DatasetStatisticsMap;
    //gene selection objects
    typedef QVector<UserExperimentPtr> UserExperimentList;
    //cell figure hashed by figure name (figure names are unique)
    typedef QMap<QString, QString> CellFigureMap;
    // selection set
    typedef QVector<GeneSelection> UniqueGeneSelectedList;

    DataProxy();
    virtual ~DataProxy();

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
    // dataset statistics
    async::DataRequest loadDatasetStatisticsByDatasetId(const QString& datasetId);
    // cell tissue figure
    async::DataRequest loadCellTissueByName(const QString& name);
    // current logged user
    async::DataRequest loadUser();
    // selection objects
    async::DataRequest loadSelectionObjects();

    //data getters
    const DatasetList& getDatasetList() const;
    GeneList& getGeneList(const QString& datasetId);
    FeatureList& getFeatureList(const QString& datasetId);
    GenePtr getGene(const QString& datasetId, const QString& geneName);
    FeatureList& getGeneFeatureList(const QString& datasetId,
                                      const QString& geneName);
    FeaturePtr getFeature(const QString& datasetId, const QString &featureId);
    DatasetPtr getDatasetById(const QString& datasetId) const;
    DatasetStatisticsPtr getStatistics(const QString& datasetId);
    ChipPtr getChip(const QString& chipId);
    UserPtr getUser() const;
    QIODevice *getFigure(const QString& figureId) const;
    const UserExperimentList& getSelectedObjects() const;
    const QString getSelectedDataset() const;

    //returns a vector of unique gene selection objects
    static UniqueGeneSelectedList getUniqueGeneSelected(const qreal roof,
                                                        const FeatureList& features);

    //setters
    void setSelectedDataset(const QString &datasetId) const;

    //updaters
    async::DataRequest updateDataset(const Dataset& dataset);

private:

    bool hasCellTissue(const QString& name) const;
    bool hasStatistics(const QString& datasetId) const;
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
    // available datasets on a list
    DatasetList m_datasetList;
    // gene mapped by dataset id and gene id
    GeneMapMap m_geneMap;
    // present genes and whether its to be shown or not
    GeneListMap m_geneListMap;
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
     // map dataset id to list of Hits
    DatasetStatisticsMap m_datasetStatisticsMap;
    // the current selection objects
    UserExperimentList m_selectedObjects;
    // the current selected dataset
    mutable QString m_selected_datasetId;

    Q_DISABLE_COPY(DataProxy)
};

#endif  /* DATAPROXY_H */
