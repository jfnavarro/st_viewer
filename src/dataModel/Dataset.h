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

    const QString& id() const { return m_id; }
    const QString& name() const { return m_name; }
    const QString& chipId() const { return m_chipId; }
    const QTransform& alignment() const { return m_alignment; }
    const QString& figureBlue() const { return m_figureBlue; }
    const QString& figureRed() const { return m_figureRed; }
    int figureStatus() const { return m_figureStatus; }
    int statBarcodes() const { return m_statBarcodes; }
    int statGenes() const { return m_statGenes; }
    int statUniqueBarcodes() const { return m_statUniqueBarcodes; }
    int statUniqueGenes() const { return m_statUniqueGenes; }
    const QString& statTissue() const { return m_statTissue; }
    const QString& statSpecie() const { return m_statSpecie; }
    const QString& statCreated() const { return m_statCreated; }
    const QString& statComments() const { return m_statComments; }

    void id(const QString& id) { m_id = id; }
    void name(const QString& name) { m_name = name; }
    void chipId(const QString& chipId) { m_chipId = chipId; }
    void alignment(const QTransform& alignment) { m_alignment = alignment; }
    void figureBlue(const QString& figure_blue) { m_figureBlue = figure_blue; }
    void figureRed(const QString& figure_red) { m_figureRed  = figure_red; }
    void figureStatus(int figure_status) { m_figureStatus = figure_status; }
    void statBarcodes(int barcodes) { m_statBarcodes = barcodes; }
    void statGenes(int genes) { m_statGenes = genes; }
    void statUniqueBarcodes(int uniqueBarcodes) { m_statUniqueBarcodes = uniqueBarcodes; }
    void statUniqueGenes(int uniqueGenes) { m_statUniqueGenes = uniqueGenes; }
    void statTissue(const QString& tissue) { m_statTissue = tissue; }
    void statSpecie(const QString& specie) { m_statSpecie = specie; }
    void statCreated(const QString& created) { m_statCreated = created; }
    void statComments(const QString& comments) { m_statComments = comments; }

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
