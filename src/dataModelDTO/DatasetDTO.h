/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef DATASETDTO_H
#define DATASETDTO_H

#include <QObject>
#include <QString>
#include <QTransform>
#include <QVariantList>

#include "dataModel/Dataset.h"

// DTOs (Data transform objects) provides the bridge between the client
// specific data model objects and the server data. This is accomplished by
// defining dynamic properties that enable automated serialization and
// deserialization of server data.

// DatasetDTO defines the parsing object for the underlying Dataset data
// object.
// Mapping Notes:
//     ~1:1 mapping. Alignment matrix is serialized into comma separated form.
//     Supports (6 and 9 component deserialization and serializes to 9
//     components).
class DatasetDTO : public QObject
{
public:

    Q_OBJECT

    Q_PROPERTY(QString id READ id WRITE id)
    Q_PROPERTY(QString name READ name WRITE name)
    Q_PROPERTY(QString chipid READ chipId WRITE chipId)
    Q_PROPERTY(QVariantList alignment_matrix READ alignment WRITE alignment)

    Q_PROPERTY(QString figure_blue READ figureBlue WRITE figureBlue)
    Q_PROPERTY(QString figure_red READ figureRed WRITE figureRed)
    Q_PROPERTY(int figure_status READ figureStatus WRITE figureStatus)

    Q_PROPERTY(int stat_barcodes READ statBarcodes WRITE statBarcodes)
    Q_PROPERTY(int stat_genes READ statGenes WRITE statGenes)
    Q_PROPERTY(int stat_unique_barcodes READ statUniqueBarcodes WRITE statUniqueBarcodes)
    Q_PROPERTY(int stat_unique_genes READ statUniqueGenes WRITE statUniqueGenes)

    Q_PROPERTY(QString stat_tissue READ statTissue WRITE statTissue)
    Q_PROPERTY(QString stat_specie READ statSpecie WRITE statSpecie)
    Q_PROPERTY(QString stat_created READ statCreated WRITE statCreated)
    Q_PROPERTY(QString stat_comments READ statComments WRITE statComments)

public:

    explicit DatasetDTO(QObject* parent = 0);
    DatasetDTO(const Dataset& dataset, QObject* parent = 0);
    virtual ~DatasetDTO();

    // binding
    const QString& id() const {  return m_dataset.id(); }
    const QString& name() const { return m_dataset.name(); }
    const QString& chipId() const { return m_dataset.chipId(); }
    const QVariantList alignment() const { return serializeTransform(m_dataset.alignment()); }
    const QString& figureBlue() const { return m_dataset.figureBlue(); }
    const QString& figureRed() const { return m_dataset.figureRed(); }
    int figureStatus() const { return m_dataset.figureStatus(); }
    int statBarcodes() const { return m_dataset.statBarcodes(); }
    int statGenes() const { return m_dataset.statGenes(); }
    int statUniqueBarcodes() const { return m_dataset.statUniqueBarcodes(); }
    int statUniqueGenes() const { return m_dataset.statUniqueGenes(); }
    const QString& statTissue() const { return m_dataset.statTissue(); }
    const QString& statSpecie() const { return m_dataset.statSpecie(); }
    const QString& statCreated() const { return m_dataset.statCreated(); }
    const QString& statComments() const { return m_dataset.statComments(); }

    void id(const QString& id) { m_dataset.id(id); }
    void name(const QString& name) { m_dataset.name(name); }
    void chipId(const QString& chipId) { m_dataset.chipId(chipId); }
    void alignment(const QVariantList& alignment) { m_dataset.alignment(unserializeTransform(alignment)); }
    void figureBlue(const QString& figure_blue) { m_dataset.figureBlue(figure_blue); }
    void figureRed(const QString& figure_red) { m_dataset.figureRed(figure_red); }
    void figureStatus(int figure_status) { m_dataset.figureStatus(figure_status); }
    void statBarcodes(int barcodes) { m_dataset.statBarcodes(barcodes); }
    void statGenes(int genes) { m_dataset.statGenes(genes); }
    void statUniqueBarcodes(int unique_barcodes) { m_dataset.statUniqueBarcodes(unique_barcodes); }
    void statUniqueGenes(int unique_genes) { m_dataset.statUniqueGenes(unique_genes); }
    void statTissue(const QString& tissue) { m_dataset.statTissue(tissue); }
    void statSpecie(const QString& specie) { m_dataset.statSpecie(specie); }
    void statCreated(const QString& created) { m_dataset.statCreated(created); }
    void statComments(const QString& comments) { m_dataset.statComments(comments); }

    // get parsed data model
    const Dataset& dataset() const { return m_dataset; }
    Dataset& dataset() { return m_dataset; }

private:

    const QTransform unserializeTransform(const QVariantList& transform) const;
    const QVariantList serializeTransform(const QTransform& transform) const;

    Dataset m_dataset;
};

#endif // DATASETDTO_H //
