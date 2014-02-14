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
#include <QScopedPointer>

#include "utils/Singleton.h"

// data objects
#include "dataModel/Chip.h"
#include "dataModel/Dataset.h"
#include "dataModel/FeatureExtended.h"
#include "dataModel/Gene.h"
#include "dataModel/DatasetStatistics.h"
#include "dataModel/User.h"
#include "dataModel/UserExperiment.h"

// network
#include "network/NetworkManager.h"
#include "network/NetworkReply.h"

// download manager / data request
#include "network/DownloadManager.h"

// DataProxy is a globally accessible all-in-all data store. It provides an
// interface to access remotely stored data and a means of storing and managing
// the transferred data locally.
// It provides the backbone for most of the data models which are in turn used
// to access specific subsets of the data store in the data proxy.
class DataProxy : public QObject, public Singleton<DataProxy>
{
    Q_OBJECT
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

    typedef QHash<uint, QPointer<async::DownloadManager> > DownLoadQueueMap;

    // MAIN CONTAINERS (MVC)
    typedef QSharedPointer<Chip> ChipPtr;
    typedef QSharedPointer<Dataset> DatasetPtr;
    typedef QSharedPointer<FeatureExtended> FeaturePtr;
    typedef QSharedPointer<Gene> GenePtr;
    typedef QSharedPointer<DatasetStatistics> DatasetStatisticsPtr;
    typedef QSharedPointer<UserExperiment> UserExperimentPtr;
    typedef QSharedPointer<User> UserPtr;

    typedef QVector<GenePtr> GeneList; //list of unique genes
    typedef QSharedPointer<GeneList> GeneListPtr; //pointer to list of unique genes
    typedef QMap<QString, GenePtr> GeneMap; //map of unique genes
    typedef QSharedPointer<GeneMap> GeneMapPtr;
    typedef QMap<QString, GeneMapPtr> GeneMapMap; //gene map hashed by dataset id
    typedef QMap<QString, GeneListPtr> GeneListMap; //gene list hashed by dataset id

    typedef QVector<FeaturePtr> FeatureList; //list of features (extended with color)
    typedef QSharedPointer<FeatureList> FeatureListPtr;
    typedef QMap<QString, FeaturePtr> FeatureMap; // map of features give feature id
    typedef QSharedPointer<FeatureMap> FeatureMapPtr;
    typedef QMap<QString, FeatureListPtr> FeatureListMap; //features hashed by dataset id
    typedef QSharedPointer<FeatureListMap> FeatureListMapPtr;
    typedef QMap<QString, FeatureMapPtr> FeatureMapMap; // feature map by dataset id
    typedef QMap<QString, FeatureListMapPtr> FeatureListGeneMap; //features hashed by dataset id and gene name

    typedef QMap<QString, ChipPtr> ChipMap; //chip hashed by dataset id

    typedef QVector<DatasetPtr> DatasetList; //list of unique datasets
    typedef QSharedPointer<DatasetList> DatasetListPtr;
    typedef QMap<QString, DatasetPtr> DatasetMap; //datasets hashed by dataset id

    typedef QMap<QString, DatasetStatisticsPtr> DatasetStatisticsMap; //hitcount hashed by dataset id

    typedef QMap<QString, UserExperimentPtr> UserExperimentMap; //NOTE not functional yet

    typedef QMap<QString, QString> CellFigureMap; //cell figure hashed by figure name (figure names are unique)

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
    async::DataRequest* loadChipById(const QString& chipId);
    // dataset
    async::DataRequest* loadDatasets();
    async::DataRequest* loadDatasetByDatasetId(const QString& datasetId);
    async::DataRequest* updateDataset(const Dataset& dataset);
    // feature
    async::DataRequest* loadFeatureByDatasetId(const QString& datasetId);
    async::DataRequest* loadFeatureByDatasetIdAndGene(const QString& datasetId, const QString& gene);
    // gene
    async::DataRequest* loadGenesByDatasetId(const QString& datasetId);
    // hit count
    async::DataRequest* loadDatasetStatisticsByDatasetId(const QString& datasetId);
    // red cell tissue figure
    async::DataRequest* loadCellTissueByName(const QString& name);
    //get the current user object
    async::DataRequest* loadUser();
    //get the the whole dataset content
    async::DataRequest* loadDatasetContent(DataProxy::DatasetPtr dataset);

    //data getters
    DatasetListPtr getDatasetList();
    GeneListPtr getGeneList(const QString& datasetId);
    FeatureListPtr getFeatureList(const QString& datasetId);
    GenePtr getGene(const QString& datasetId, const QString& geneName);
    FeatureListPtr getGeneFeatureList(const QString& datasetId, const QString& geneName);
    FeaturePtr getFeature(const QString& datasetId, const QString &featureId);
    DatasetPtr getDatasetById(const QString& datasetId);
    DatasetStatisticsPtr getStatistics(const QString& datasetId);
    ChipPtr getChip(const QString& chipId);
    UserPtr getUser();
    QIODevice *getFigure(const QString& figureId);

    const QString& getSelectedDataset() const {  return m_selected_datasetId; }
    void setSelectedDataset(const QString &datasetId) const { m_selected_datasetId = datasetId; }

private slots:

    void slotNetworkReply(QVariant code, QVariant data);

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
    Error* parseErrors(NetworkReply* reply);
    bool parseData(NetworkReply* reply, const QVariantMap& map);

    //internal function to create network requests trough download manager
    async::DataRequest* createRequest(NetworkReply *reply);
    async::DataRequest* createRequest(const QList<NetworkReply*> &replies);

    DownLoadQueueMap m_download_pool;           // we store here every active download manager item with a hash key

    DatasetMap m_datasetMap;                    // available datasets mapped by dataset id
    DatasetListPtr m_datasetListPtr;            // available datasets on a list
    GeneMapMap m_geneMap;                       // gene mapped by dataset id and gene id
    GeneListMap m_geneListMap;                  // present genes and whether its to be shown or not
    ChipMap m_chipMap;                          // chip arrays mapped by dataset id
    FeatureMapMap m_featureMap;                 // feature mapped by dataset id and feature id
    FeatureListMap m_featureListMap;            // map dataset id to list of features
    FeatureListGeneMap m_geneFeatureListMap;    // map dataset id and gene name to list of features
    UserPtr m_user;                             // the current user logged in
    DatasetStatisticsMap m_datasetStatisticsMap;                  // map dataset id to list of Hits

    mutable QString m_selected_datasetId;       // the current selected dataset

protected:

    GeneMapPtr getGeneMap(const QString &datasetId);
    FeatureMapPtr getFeatureMap(const QString &datasetId);
};

#endif  /* DATAPROXY_H */
