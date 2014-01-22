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

    enum Status {
        Empty   = 0x00, // No flags set
        Aligned = 0x01  // Dataset has been aligned with associated image
    };

    Dataset();
    explicit Dataset(const Dataset& other);
    virtual ~Dataset();

    Dataset& operator= (const Dataset& other);
    bool operator== (const Dataset& other) const;

    inline const QString& id() const { return m_id; }
    inline const QString& name() const { return m_name; }
    inline const QString& chipId() const { return m_chipId; }
    inline const QTransform& alignment() const { return m_alignment; }
    inline const QString& figureBlue() const { return m_figureBlue; }
    inline const QString& figureRed() const { return m_figureRed; }
    inline int figureStatus() const { return m_figureStatus; }
    inline int statBarcodes() const { return m_statBarcodes; }
    inline int statGenes() const { return m_statGenes; }
    inline int statUniqueBarcodes() const { return m_statUniqueBarcodes; }
    inline int statUniqueGenes() const { return m_statUniqueGenes; }
    inline const QString& statTissue() const { return m_statTissue; }
    inline const QString& statSpecie() const { return m_statSpecie; }
    inline const QString& statCreated() const { return m_statCreated; }
    inline const QString& statComments() const { return m_statComments; }

    inline void id(const QString& id) { m_id = id; }
    inline void name(const QString& name) { m_name = name; }
    inline void chipId(const QString& chipId) { m_chipId = chipId; }
    inline void alignment(const QTransform& alignment) { m_alignment = alignment; }
    inline void figureBlue(const QString& figure_blue) { m_figureBlue = figure_blue; }
    inline void figureRed(const QString& figure_red) { m_figureRed  = figure_red; }
    inline void figureStatus(int figure_status) { m_figureStatus = figure_status; }
    inline void statBarcodes(int barcodes) { m_statBarcodes = barcodes; }
    inline void statGenes(int genes) { m_statGenes = genes; }
    inline void statUniqueBarcodes(int uniqueBarcodes) { m_statUniqueBarcodes = uniqueBarcodes; }
    inline void statUniqueGenes(int uniqueGenes) { m_statUniqueGenes = uniqueGenes; }
    inline void statTissue(const QString& tissue) { m_statTissue = tissue; }
    inline void statSpecie(const QString& specie) { m_statSpecie = specie; }
    inline void statCreated(const QString& created) { m_statCreated = created; }
    inline void statComments(const QString& comments) { m_statComments = comments; }

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
