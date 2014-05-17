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

    const QString id() const { return m_id; }
    const QString name() const { return m_name;}
    const QString imageAlignmentId() const { return m_alignmentId; }
    int statBarcodes() const { return m_statBarcodes; }
    int statGenes() const { return m_statGenes; }
    int statUniqueBarcodes() const { return m_statUniqueBarcodes; }
    int statUniqueGenes() const { return m_statUniqueGenes; }
    const QString statTissue() const { return m_statTissue; }
    const QString statSpecie() const { return m_statSpecie; }
    const QString statComments() const { return m_statComments; }
    const QList<QString> oboFoundryTerms() const { return m_oboFroundryTerms; }
    const QVector<qreal> hitsQuartiles() const { return m_genePooledHitsQuartiles; }

    void id(const QString& id) { m_id = id; }
    void name(const QString& name) { m_name = name; }
    void imageAlignmentId(const QString& alignmentId) { m_alignmentId = alignmentId; }
    void statBarcodes(int barcodes) { m_statBarcodes = barcodes; }
    void statGenes(int genes) { m_statGenes = genes; }
    void statUniqueBarcodes(int uniqueBarcodes) { m_statUniqueBarcodes = uniqueBarcodes; }
    void statUniqueGenes(int uniqueGenes) { m_statUniqueGenes = uniqueGenes; }
    void statTissue(const QString& statTissue) { m_statTissue = statTissue; }
    void statSpecie(const QString& statSpecie) { m_statSpecie = statSpecie; }
    void statComments(const QString& statComments) { m_statComments = statComments; }
    void oboFoundryTerms(const QList<QString>& oboFoundryTerms) { m_oboFroundryTerms = oboFoundryTerms;}
    void hitsQuartiles(const QVector<qreal>& hitsQuartiles) { m_genePooledHitsQuartiles = hitsQuartiles; }

    //extended
    qreal min() const;
    qreal max() const;
    qreal pooledMin() const;
    qreal pooledMax() const;

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
};

#endif // DATASET_H
