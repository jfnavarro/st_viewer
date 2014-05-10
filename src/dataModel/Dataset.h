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
    virtual ~Dataset();

    Dataset& operator= (const Dataset& other);
    bool operator== (const Dataset& other) const;

    const QString& id() const;
    const QString& name() const;
    const QString& imageAlignmentId() const;
    int statBarcodes() const;
    int statGenes() const;
    int statUniqueBarcodes() const;
    int statUniqueGenes() const;
    const QString& statTissue() const;
    const QString& statSpecie() const;
    const QString& statComments() const;
    const QList<QString>& oboFoundryTerms() const;
    const QVector<qreal,4>& hitsQuartiles() const;

    const QString& id() const;
    const QString& name() const;
    const QString& imageAlignmentId() const;
    int statBarcodes() const;
    int statGenes() const;
    int statUniqueBarcodes() const;
    int statUniqueGenes() const;
    const QString& statTissue() const;
    const QString& statSpecie() const;
    const QString& statComments() const;
    const QList<QString>& oboFoundryTerms() const;
    const QVector<qreal,4>& hitsQuartiles() const;

private:

    QString m_id;
    QString m_name;
    QString m_chipId;
    QTransform m_alignment;
    QString m_figureBlue;   // file name
    QString m_figureRed;    // file name
    int m_figureStatus;
    int m_statBarcodes;
    int m_statGenes;
    int m_statUniqueBarcodes;
    int m_statUniqueGenes;
    QString m_statTissue;
    QString m_statSpecie;
    QString m_statCreated;
    QString m_statComments;
};

#endif // DATASET_H
