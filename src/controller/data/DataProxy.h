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
class DataProxy : public QObject, public Singleton<DataProxy>
{
    Q_OBJECT
    Q_ENUMS(DataType)
        
public:
    
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
    typedef QSharedPointer<Chip> ChipPtr;
    typedef QSharedPointer<Dataset> DatasetPtr;
    typedef QSharedPointer<FeatureExtended> FeaturePtr;
    typedef QSharedPointer<Gene> GenePtr;
    typedef QSharedPointer<HitCount> HitCountPtr;
    typedef QSharedPointer<UserExperiment> UserExperimentPtr;
    typedef QSharedPointer<User> UserPtr;

    typedef QVector<GenePtr> GeneList; //list of unique genes
    typedef QSharedPointer<GeneList> GeneListPtr; //pointer to list of unique genes
    typedef QMap<QString, GenePtr> GeneMap; //map of unique genes
    typedef QMap<QString, GeneMap> GeneMapMap; //gene map hashed by dataset id
    typedef QMap<QString, GeneListPtr> GeneListMap; //gene list hashed by dataset id
      
    typedef QVector<FeaturePtr> FeatureList; //list of features (extended with color)
    typedef QSharedPointer<FeatureList> FeatureListPtr;    
    typedef QMap<QString, FeaturePtr> FeatureMap; // map of features give feature id
    typedef QMap<QString, FeatureListPtr> FeatureListMap; //features hashed by dataset id
    typedef QMap<QString, FeatureMap> FeatureMapMap; // feature map by dataset id
    typedef QMap<QString, FeatureListMap> FeatureListGeneMap; //features hashed by dataset id and gene name
    
    typedef QMap<QString, ChipPtr> ChipMap; //chip hashed by dataset id
    
    typedef QVector<DatasetPtr> DatasetList; //list of unique datasets
    typedef QSharedPointer<DatasetList> DatasetListPtr;
    typedef QMap<QString, DatasetPtr> DatasetMap; //datasets hashed by dataset id
     
    typedef QMap<QString, HitCountPtr> HitCountMap; //hitcount hashed by dataset id
    
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
    async::DataRequest* loadHitCountByDatasetId(const QString& datasetId);    
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
    GenePtr getGeneByIndex(const QString& datasetId, const int index);
    FeatureListPtr getGeneFeatureList(const QString& datasetId, const QString& geneName);
    FeaturePtr getFeature(const QString& datasetId, const QString &featureId);
    DatasetPtr getDatasetById(const QString& datasetId);
    HitCountPtr getHitCount(const QString& datasetId);
    
    ChipPtr getChip(const QString& chipId);
    UserPtr getUser();
    QIODevice *getFigure(const QString& figureId); 
    
    inline const QString& getSelectedDataset() const { return m_selected_datasetId; }
    inline void setSelectedDataset(const QString &datasetId) const { m_selected_datasetId = datasetId; }
    
private slots:
    
    void slotNetworkReply(QVariant code, QVariant data);
    
private:
    
    Q_DISABLE_COPY(DataProxy);
    
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
    async::DataRequest* createRequest(QList<NetworkReply*> replies);
 
    DownLoadQueueMap m_download_pool; // we store here every active download manager item with a hash key

    DatasetMap m_datasetMap;                    // available datasets mapped by dataset id
    GeneMapMap m_geneMap;                       // gene mapped by dataset id and gene id
    GeneListMap m_geneListMap;                  // present genes and whether its to be shown or not
    ChipMap m_chipMap;                          // chip arrays mapped by dataset id
    FeatureMapMap m_featureMap;                 // feature mapped by dataset id and feature id
    FeatureListMap m_featureListMap;            // map dataset id to list of features
    FeatureListGeneMap m_geneFeatureListMap;    // map dataset id and gene name to list of features
    UserExperimentMap m_userExperimentMap;      // user experiments mapped by dataset id
    UserPtr m_user;                             // the current user logged in
    HitCountMap m_hitCountMap;                  // map dataset id to list of Hits
    
    mutable QString m_selected_datasetId;       // the current selected dataset

};

#endif	/* DATAPROXY_H */
