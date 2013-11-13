/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef DATAPROXY_H
#define	DATAPROXY_H

#include <QObject>
#include <QVector>
#include <QMap>
#include <QScopedPointer>
// images
#include "controller/async/ImageProcessing.h"
// patterns
#include "utils/Singleton.h"
// data objects
#include "model/Chip.h"
#include "model/Dataset.h"
#include "model/FeatureExtended.h"
#include "model/Gene.h"
#include "model/HitCount.h"
#include "model/User.h"
#include "model/UserExperiment.h"
// network
#include "controller/network/NetworkManager.h"
#include "controller/network/NetworkReply.h"
// download manager / data request
#include "controller/network/DownloadManager.h"

// DataProxy is a globally accessible all-in-all data store. It provides an
// interface to access remotely stored data and a means of storing and managing
// the transferred data locally.
// It provides the backbone for most of the data models which are in turn used
// to access specific subsets of the data store in the data proxy.

template<typename T, typename U, typename P>
static const T& getContainer(const U& container)
{
    T newcontainer;
    foreach(P ele, container)
    {
        newcontainer << ele.get();
    }
    return newcontainer;
}

class DataProxy : public QObject, public Singleton<DataProxy>
{
    Q_OBJECT
    Q_ENUMS(DataType)

protected:
    
    enum DataType
    {
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
    typedef std::unique_ptr<Chip> ChipPtr;
    typedef std::unique_ptr<Dataset> DatasetPtr;
    typedef std::unique_ptr<FeatureExtended> FeaturePtr;
    typedef std::unique_ptr<Gene> GenePtr;
    typedef std::unique_ptr<HitCount> HitCountPtr;
    typedef std::unique_ptr<UserExperiment> UserExperimentPtr;
    typedef std::unique_ptr<User> UserPtr;

    typedef QVector<GenePtr> GeneList; //list of unique genes
    //typedef std::unique_ptr<GeneList> GeneListPtr; //pointer to list of unique genes
    typedef QMap<QString, GenePtr> GeneMap; //map of unique genes
    //typedef std::unique_ptr<GeneMap> GeneMapPtr; //pointer to a map of unique genes
    //typedef QMap<QString, GeneMapPtr> GeneMapMap; //gene map hashed by dataset id
    //typedef QMap<QString, GeneListPtr> GeneListMap; //gene list hashed by dataset id
    typedef QMap<QString, GeneMap> GeneMapMap; //gene map hashed by dataset id
    typedef QMap<QString, GeneList> GeneListMap; //gene list hashed by dataset id

    typedef QVector<FeaturePtr> FeatureList; //list of features (extended with color)
    //typedef std::unique_ptr<FeatureList> FeatureListPtr; //pointer to a list of features
    typedef QMap<QString, FeaturePtr> FeatureMap; // map of features give feature id
    //typedef std::unique_ptr<FeatureMap> FeatureMapPtr; //pointer to a map of features/id
    //typedef QMap<QString, FeatureListPtr> FeatureListMap; //features hashed by dataset id
    typedef QMap<QString, FeatureList> FeatureListMap; //features hashed by dataset id
    //typedef std::unique_ptr<FeatureListMap> FeatureListMapPtr; //pointer to  map of datasetid/featurelist
    //typedef QMap<QString, FeatureMapPtr> FeatureMapMap; // feature map by dataset id
    //typedef QMap<QString, FeatureListMapPtr> FeatureListGeneMap; //features hashed by dataset id and gene name
    typedef QMap<QString, FeatureMap> FeatureMapMap; // feature map by dataset id
    typedef QMap<QString, FeatureListMap> FeatureListGeneMap; //features hashed by dataset id and gene name

    typedef QMap<QString, ChipPtr> ChipMap; //chip hashed by dataset id
    
    typedef QVector<DatasetPtr> DatasetList; //list of unique datasets
    //typedef std::unique_ptr<DatasetList> DatasetListPtr; //pointer to a list of unique datasets
    typedef QMap<QString, DatasetPtr> DatasetMap; //datasets hashed by dataset id

    typedef QMap<QString, HitCountPtr> HitCountMap; //hitcount hashed by dataset id
    
    typedef QMap<QString, UserExperimentPtr> UserExperimentMap; //NOTE not functional yet
    
    typedef QMap<QString, QString> CellFigureMap; //cell figure hashed by figure name (figure names are unique)
    
public:

    typedef Chip* ChipRef;
    typedef Dataset* DatasetRef;
    typedef FeatureExtended* FeatureRef;
    typedef Gene* GeneRef;
    typedef HitCount* HitCountRef;
    typedef UserExperiment* UserExperimentRef;
    typedef User* UserRef;

    typedef QVector<GeneRef> GeneListRef; //list of unique genes
    typedef QVector<FeatureRef> FeatureListRef; //list of features (extended with color)
    typedef QVector<DatasetRef> DatasetListRef; //list of unique datasets

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
    async::DataRequest* loadHitCountByDatasetId(const QString& datasetId);
    // red cell tissue figure
    async::DataRequest* loadCellTissueByName(const QString& name);
    //get the current user object
    async::DataRequest* loadUser();
    //get the the whole dataset content
    async::DataRequest* loadDatasetContent(DataProxy::DatasetRef dataset);

    //data getters
    const DatasetListRef& getDatasetListRef() const;
    const GeneListRef& getGeneListRef(const QString& datasetId);
    const FeatureListRef& getFeatureListRef(const QString& datasetId);
    const FeatureListRef& getGeneFeatureListRef(const QString& datasetId, const QString& geneName);

    //TODO not used I think
    GeneRef getGeneRef(const QString& datasetId, const QString& geneName);
    //GeneRef getGeneByIndex(const QString& datasetId, const int index);
    FeatureRef getFeatureRef(const QString& datasetId, const QString &featureId);

    DatasetRef getDatasetRefById(const QString& datasetId);
    HitCountRef getHitCountRef(const QString& datasetId);
    ChipRef getChipRef(const QString& chipId);
    UserRef getUserRef();

    QIODevice *getFigure(const QString& figureId);
    
    inline const QString& getSelectedDataset() const { return m_selected_datasetId; }
    inline void setSelectedDataset(const QString &datasetId) const { m_selected_datasetId = datasetId; }
    
private slots:
    
    void slotNetworkReply(QVariant code, QVariant data);
    
private:

    const GeneList& getGeneList(const QString& datasetId);
    const GeneMap& getGeneMap(const QString &datasetId);
    const FeatureMap& getFeatureMap(const QString &datasetId);
    const FeatureList& getFeatureList(const QString& datasetId);
    const FeatureList& getGeneFeatureList(const QString& datasetId, const QString& geneName);

    bool hasCellTissue(const QString& name) const;
    bool hasHitCount(const QString& datasetId) const;
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

    DownLoadQueueMap m_download_pool; // we store here every active download manager item with a hash key

    DatasetMap m_datasetMap;                    // available datasets mapped by dataset id
    DatasetList m_datasetList;                  // available datasets on a list

    GeneMapMap m_geneMap;                       // gene mapped by dataset id and gene id
    GeneListMap m_geneListMap;                  // present genes and whether its to be shown or not

    ChipMap m_chipMap;                          // chip arrays mapped by dataset id

    FeatureMapMap m_featureMap;                 // feature mapped by dataset id and feature id
    FeatureListMap m_featureListMap;            // map dataset id to list of features
    FeatureListGeneMap m_geneFeatureListMap;    // map dataset id and gene name to list of features

    UserPtr m_user;                             // the current user logged in

    HitCountMap m_hitCountMap;                  // map dataset id to list of Hits
    
    mutable QString m_selected_datasetId;       // the current selected dataset 

};

#endif	/* DATAPROXY_H */
