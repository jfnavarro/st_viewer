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

    //list of unique genes
    typedef QVector<GenePtr> GeneList;
    typedef QSharedPointer<GeneList> GeneListPtr;
    //map of unique genes
    typedef QMap<QString, GenePtr> GeneMap;
    typedef QSharedPointer<GeneMap> GeneMapPtr;
    //gene map hashed by dataset id
    typedef QMap<QString, GeneMapPtr> GeneMapMap;
    //gene list hashed by dataset id
    typedef QMap<QString, GeneListPtr> GeneListMap;
    //list of features (extended with color)
    typedef QVector<FeaturePtr> FeatureList;
    typedef QSharedPointer<FeatureList> FeatureListPtr;
    // map of features give feature id
    typedef QMap<QString, FeaturePtr> FeatureMap;
    typedef QSharedPointer<FeatureMap> FeatureMapPtr;
    //features hashed by dataset id
    typedef QMap<QString, FeatureListPtr> FeatureListMap;
    typedef QSharedPointer<FeatureListMap> FeatureListMapPtr;
    // feature map by dataset id
    typedef QMap<QString, FeatureMapPtr> FeatureMapMap;
    //features hashed by dataset id and gene name
    typedef QMap<QString, FeatureListMapPtr> FeatureListGeneMap;
    //chip hashed by dataset id
    typedef QMap<QString, ChipPtr> ChipMap;
    //list of unique datasets
    typedef QVector<DatasetPtr> DatasetList;
    typedef QSharedPointer<DatasetList> DatasetListPtr;
    //datasets hashed by dataset id
    typedef QMap<QString, DatasetPtr> DatasetMap;
    //hitcount hashed by dataset id
    typedef QMap<QString, DatasetStatisticsPtr> DatasetStatisticsMap;
    //NOTE not functional yet
    typedef QMap<QString, UserExperimentPtr> UserExperimentMap;
    //cell figure hashed by figure name (figure names are unique)
    typedef QMap<QString, QString> CellFigureMap;

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
    const async::DataRequest* loadChipById(const QString& chipId);
    // dataset
    const async::DataRequest* loadDatasets();
    const async::DataRequest* loadDatasetByDatasetId(const QString& datasetId);
    const async::DataRequest* updateDataset(const Dataset& dataset);
    // feature
    const async::DataRequest* loadFeatureByDatasetId(const QString& datasetId);
    const async::DataRequest* loadFeatureByDatasetIdAndGene(const QString& datasetId,
                                                      const QString& gene);
    // gene
    const async::DataRequest* loadGenesByDatasetId(const QString& datasetId);
    // hit count
    const async::DataRequest* loadDatasetStatisticsByDatasetId(const QString& datasetId);
    // red cell tissue figure
    const async::DataRequest* loadCellTissueByName(const QString& name);
    //get the current user object
    const async::DataRequest* loadUser();
    //get the the whole dataset content
    const async::DataRequest* loadDatasetContent(DataProxy::DatasetPtr dataset);

    //data getters
    DatasetListPtr getDatasetList() const;
    GeneListPtr getGeneList(const QString& datasetId);
    FeatureListPtr getFeatureList(const QString& datasetId);
    GenePtr getGene(const QString& datasetId, const QString& geneName);
    FeatureListPtr getGeneFeatureList(const QString& datasetId,
                                      const QString& geneName);
    FeaturePtr getFeature(const QString& datasetId, const QString &featureId);
    DatasetPtr getDatasetById(const QString& datasetId) const;
    DatasetStatisticsPtr getStatistics(const QString& datasetId);
    ChipPtr getChip(const QString& chipId);
    UserPtr getUser() const;
    QIODevice *getFigure(const QString& figureId) const;

    const QString getSelectedDataset() const;
    void setSelectedDataset(const QString &datasetId) const;

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
    const async::DataRequest* createRequest(NetworkReply *reply);
    const async::DataRequest* createRequest(const QList<NetworkReply *> &replies);

    GeneMapPtr getGeneMap(const QString &datasetId);
    FeatureMapPtr getFeatureMap(const QString &datasetId);

    // we store here every active download manager item with a hash key
    DownLoadQueueMap m_download_pool;
    // available datasets mapped by dataset id
    DatasetMap m_datasetMap;
    // available datasets on a list
    DatasetListPtr m_datasetListPtr;
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
    // the current selected dataset
    mutable QString m_selected_datasetId;

    Q_DISABLE_COPY(DataProxy)
};

#endif  /* DATAPROXY_H */
