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
#include <QSharedPointer>
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
    
    // MAIN CONTAINERS (MVC)
    typedef QSharedPointer<Chip> ChipPtr;
    typedef QSharedPointer<Dataset> DatasetPtr;
    typedef QSharedPointer<FeatureExtended> FeaturePtr;
    typedef QSharedPointer<Gene> GenePtr;
    typedef QSharedPointer<HitCount> HitCountPtr;
    typedef QSharedPointer<UserExperiment> UserExperimentPtr;
    typedef QSharedPointer<User> UserPtr;

    typedef QMap<QString, FeaturePtr> FeatureMap; // map of features give feature id
    typedef QSharedPointer<FeatureMap> FeatureMapPtr;
    typedef QVector<GenePtr> GeneList; //list of unique genes
    typedef QSharedPointer<GeneList> GeneListPtr;
    typedef QMap<QString, GenePtr> GeneMap; //map of unique genes
    typedef QSharedPointer<GeneMap> GeneMapPtr;
    typedef QVector<FeaturePtr> FeatureList; //list of features (extended with color)
    typedef QSharedPointer<FeatureList> FeatureListPtr;
    typedef QMap<QString, FeatureListPtr> FeatureListMap; //features hashed by dataset id
    typedef QSharedPointer<FeatureListMap> FeatureListMapPtr;

    typedef QMap<QString, ChipPtr> ChipMap; //chip hashed by dataset id
    typedef QMap<QString, DatasetPtr> DatasetMap; //datasets hashed by dataset id
    typedef QSharedPointer<DatasetMap> DatasetMapPtr;
    typedef QMap<QString, GeneMapPtr> GeneMapMap; //gene map hashed by dataset id
    typedef QMap<QString, GeneListPtr> GeneListMap; //gene list hashed by dataset id
    typedef QMap<QString, FeatureMapPtr> FeatureMapMap; // feature map by dataset id
    typedef QMap<QString, FeatureListMapPtr> FeatureListGeneMap; //features hashed by dataset id and gene name
    typedef QMap<QString, HitCountPtr> HitCountMap; //hitcount hashed by dataset id
    typedef QMap<QString, UserExperimentPtr> UserExperimentMap; //NOTE not functional yet
    typedef QMap<QString, QString> CellFigureMap; //cell figure hashed by figure name (figure names are unique)
    
    DataProxy();
    virtual ~DataProxy();

    //init instance
    void init();
    //remove instances
    void finalize();
    //clean up cache
    void clean();
    //clean up cache and files
    void cleanAll();
    
    /* REST API */
    
    // chip
    bool hasChip(const QString& chipId) const;
    async::DataRequest* loadChipById(const QString& chipId);

    // dataset
    bool hasDatasets() const;
    bool hasDataset(const QString& datasetId) const;
    async::DataRequest* loadDatasets();
    async::DataRequest* loadDatasetByDatasetId(const QString& datasetId);
    async::DataRequest* updateDataset(const Dataset& dataset);
    
    // feature
    bool hasFeature(const QString& datasetId) const;
    bool hasFeature(const QString& datasetId, const QString& gene) const;

    async::DataRequest* loadFeatureByDatasetId(const QString& datasetId);
    async::DataRequest* loadFeatureByDatasetIdAndGene(const QString& datasetId, const QString& gene);
    
    // gene
    bool hasGene(const QString& datasetId) const;
    async::DataRequest* loadGenesByDatasetId(const QString& datasetId);
    
    // hit count
    bool hasHitCount(const QString& datasetId) const;
    async::DataRequest* loadHitCountByDatasetId(const QString& datasetId);
    
    // red cell tissue figure
    bool hasCellTissue(const QString& name) const;
    async::DataRequest* loadCellTissueByName(const QString& name);
    
    //get the current user object
    async::DataRequest* loadUser();
    
    //get the the whole dataset content
    async::DataRequest* loadDatasetContent(DataProxy::DatasetPtr dataset);

    //Some getters
    GeneMapPtr getGeneMap(const QString& datasetId);
    GeneListPtr getGeneList(const QString& datasetId);
    FeatureListPtr getFeatureList(const QString& datasetId);
    FeatureMapPtr getFeatureMap(const QString& datasetId);
    GenePtr getGene(const QString& datasetId, const QString& geneName);
    FeatureListPtr getGeneFeatureList(const QString& datasetId, const QString& geneName);
    FeaturePtr getFeature(const QString& datasetId, const QString &featureId);
    ChipPtr getChip(const QString& chipId);
    DatasetMapPtr getDatasetMap();
    DatasetPtr getDatasetById(const QString& datasetId);
    UserPtr getUser();
    HitCountPtr getHitCount(const QString& datasetId);
    QIODevice *getFigure(const QString& figureId); 
    
private slots:
    
    void slotNetworkReply(QVariant code, QVariant data);
    
private:
        
    // internal functions to parse network reply
    Error* parseErrors(NetworkReply* reply);
    bool parseData(NetworkReply* reply, const QVariantMap& map);
    async::DataRequest* createRequest(NetworkReply *reply);
    async::DataRequest* createRequest(QList<NetworkReply*> replies);
 
    QHash<uint,QPointer<async::DownloadManager> > m_download_pool; // we store here every active download manager item with a hash key

    QSharedPointer<DatasetMap> m_datasetMap;                    // available datasets mapped by dataset id
    QSharedPointer<GeneMapMap> m_geneMap;                       // gene mapped by dataset id and gene id
    QSharedPointer<GeneListMap> m_geneListMap;                  // present genes and whether its to be shown or not
    QSharedPointer<ChipMap> m_chipMap;                          // chip arrays mapped by dataset id
    QSharedPointer<FeatureMapMap> m_featureMap;                 // feature mapped by dataset id and feature id
    QSharedPointer<FeatureListMap> m_featureListMap;            // map dataset id to list of features
    QSharedPointer<FeatureListGeneMap> m_geneFeatureListMap;    // map dataset id and gene name to list of features
    QSharedPointer<UserExperimentMap> m_userExperimentMap;      // user experiments mapped by dataset id
    QSharedPointer<User> m_user;                                // the current user logged in
    QSharedPointer<HitCountMap> m_hitCountMap;                  // map dataset id to list of Hits

};

#endif	/* DATAPROXY_H */
