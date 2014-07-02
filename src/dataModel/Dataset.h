/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef DATASET_H
#define DATASET_H

#include <QString>
#include <QTransform>

// Data model class to store dataset data. Each dataset has a status attribute
// containing various flags. Currently the only flag defined is the alignment
// flag.
class Dataset
{

public:

    Dataset();
    explicit Dataset(const Dataset& other);
    ~Dataset();

    Dataset& operator= (const Dataset& other);
    bool operator== (const Dataset& other) const;

    const QString id() const;
    const QString name() const;
    const QString imageAlignmentId() const;
    int statBarcodes() const;
    int statGenes() const;
    int statUniqueBarcodes() const;
    int statUniqueGenes() const;
    const QString statTissue() const;
    const QString statSpecie() const;
    const QString statComments() const;
    const QList<QString> oboFoundryTerms() const;
    const QVector<qreal> hitsQuartiles() const;
    bool enabled() const;

    void id(const QString& id);
    void name(const QString& name);
    void imageAlignmentId(const QString& alignmentId);
    void statBarcodes(int barcodes);
    void statGenes(int genes);
    void statUniqueBarcodes(int uniqueBarcodes);
    void statUniqueGenes(int uniqueGenes);
    void statTissue(const QString& statTissue);
    void statSpecie(const QString& statSpecie);
    void statComments(const QString& statComments);
    void oboFoundryTerms(const QList<QString>& oboFoundryTerms);
    void hitsQuartiles(const QVector<qreal>& hitsQuartiles);
    void enabled(const bool enabled);

    //extended methods to get statistics
    qreal statisticsMin() const;
    qreal statisticsMax() const;
    qreal statisticsPooledMin() const;
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
    QString m_statSpecie;
    QString m_statComments;
    QList<QString> m_oboFroundryTerms;
    QVector<qreal> m_genePooledHitsQuartiles;
    bool m_enabled;
};

#endif // DATASET_H
