#ifndef DATASET_H
#define DATASET_H

#include <QString>
#include <QTransform>

// Data model class to store datasets.
class Dataset
{

public:
    Dataset();
    explicit Dataset(const Dataset &other);
    ~Dataset();

    Dataset &operator=(const Dataset &other);
    bool operator==(const Dataset &other) const;
    // Id is the DataBase id
    const QString id() const;
    const QString name() const;
    // reference to image alignment object Id
    const QString imageAlignmentId() const;
    // some stats of the dataset
    int statBarcodes() const;
    int statGenes() const;
    int statUniqueBarcodes() const;
    int statUniqueGenes() const;
    const QString statTissue() const;
    const QString statSpecies() const;
    const QString statComments() const;
    // some stats about the reads distributions
    const QVector<float> hitsQuartiles() const;
    const QVector<float> hitsPooledQuartiles() const;
    // the dataset is enabled or not
    bool enabled() const;
    // accounts that can access it
    const QVector<QString> grantedAccounts() const;
    const QString createdByAccount() const;
    const QString created() const;
    const QString lastModified() const;
    // the dataset has been downloaded from the cloud (true) or locally imported
    // (false)
    bool downloaded() const;

    void id(const QString &id);
    void name(const QString &name);
    void imageAlignmentId(const QString &alignmentId);
    void statBarcodes(int barcodes);
    void statGenes(int genes);
    void statUniqueBarcodes(int uniqueBarcodes);
    void statUniqueGenes(int uniqueGenes);
    void statTissue(const QString &statTissue);
    void statSpecies(const QString &statSpecies);
    void statComments(const QString &statComments);
    void hitsQuartiles(const QVector<float> &hitsQuartiles);
    void hitsPooledQuartiles(const QVector<float> &hitsPooledQuartiles);
    void enabled(const bool enabled);
    void grantedAccounts(const QVector<QString> &grantedAccounts);
    void createdByAccount(const QString &created);
    void created(const QString &created);
    void lastModified(const QString &lastModified);
    void downloaded(const bool downloaded);

    // some helper methods to get statistics
    // returns the first quartile
    float statisticsMin() const;
    // returns the adjusted fourth quartile (account for sparse distributions)
    float statisticsMax() const;
    // returns the first quartile
    float statisticsPooledMin() const;
    // returns the adjusted fourth quartile (account for sparse distributions)
    float statisticsPooledMax() const;

private:
    QString m_id;
    QString m_name;
    QString m_alignmentId;
    int m_statBarcodes;
    int m_statGenes;
    int m_statUniqueBarcodes;
    int m_statUniqueGenes;
    QString m_statTissue;
    QString m_statSpecies;
    QString m_statComments;
    QVector<QString> m_oboFroundryTerms;
    QVector<float> m_geneHitsQuartiles;
    QVector<float> m_genePooledHitsQuartiles;
    bool m_enabled;
    QVector<QString> m_grantedAccounts;
    QString m_createdByAccount;
    QString m_created;
    QString m_lastMofidied;
    bool m_downloaded;
};

#endif // DATASET_H
