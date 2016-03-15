#ifndef DATASET_H
#define DATASET_H

#include <QString>
#include <QTransform>

// Data model class to store datasets.
class Dataset
{

public:
    Dataset();
    explicit Dataset(const Dataset& other);
    ~Dataset();

    Dataset& operator=(const Dataset& other);
    bool operator==(const Dataset& other) const;
    // Id is the DB id
    const QString id() const;
    const QString name() const;
    // reference to image alignment object
    const QString imageAlignmentId() const;
    // some stats of the dataset
    int statBarcodes() const;
    int statGenes() const;
    int statUniqueBarcodes() const;
    int statUniqueGenes() const;
    const QString statTissue() const;
    const QString statSpecies() const;
    const QString statComments() const;
    const QVector<QString> oboFoundryTerms() const;
    // more stats about the reads distributions
    const QVector<qreal> hitsQuartiles() const;
    const QVector<qreal> hitsPooledQuartiles() const;
    bool enabled() const;
    const QVector<QString> grantedAccounts() const;
    const QString createdByAccount() const;
    const QString created() const;
    const QString lastModified() const;
    bool downloaded() const;

    void id(const QString& id);
    void name(const QString& name);
    void imageAlignmentId(const QString& alignmentId);
    void statBarcodes(int barcodes);
    void statGenes(int genes);
    void statUniqueBarcodes(int uniqueBarcodes);
    void statUniqueGenes(int uniqueGenes);
    void statTissue(const QString& statTissue);
    void statSpecies(const QString& statSpecies);
    void statComments(const QString& statComments);
    void oboFoundryTerms(const QVector<QString>& oboFoundryTerms);
    void hitsQuartiles(const QVector<qreal>& hitsQuartiles);
    void hitsPooledQuartiles(const QVector<qreal>& hitsPooledQuartiles);
    void enabled(const bool enabled);
    void grantedAccounts(const QVector<QString>& grantedAccounts);
    void createdByAccount(const QString& created);
    void created(const QString& created);
    void lastModified(const QString& lastModified);
    void downloaded(const bool downloaded);

    // extended methods to get statistics
    // returns the first quartile
    qreal statisticsMin() const;
    // returns the adjusted fourth quartile (account for sparse distributions)
    qreal statisticsMax() const;
    // returns the first quartile
    qreal statisticsPooledMin() const;
    // returns the adjusted fourth quartile (account for sparse distributions)
    qreal statisticsPooledMax() const;

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
    QVector<qreal> m_geneHitsQuartiles;
    QVector<qreal> m_genePooledHitsQuartiles;
    bool m_enabled;
    QVector<QString> m_grantedAccounts;
    QString m_createdByAccount;
    QString m_created;
    QString m_lastMofidied;
    bool m_downloaded;
};

#endif // DATASET_H
