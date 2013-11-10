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

#include "model/Dataset.h"

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
    inline const QString& id() const { return m_dataset.id(); }
    inline const QString& name() const { return m_dataset.name(); }
    inline const QString& chipId() const { return m_dataset.chipId(); }
    inline const QVariantList alignment() const { return serializeTransform(m_dataset.alignment()); }
    inline const QString& figureBlue() const { return m_dataset.figureBlue(); }
    inline const QString& figureRed() const { return m_dataset.figureRed(); }
    inline int figureStatus() const { return m_dataset.figureStatus(); }
    inline int statBarcodes() const {return m_dataset.statBarcodes();}
    inline int statGenes() const {return m_dataset.statGenes();}
    inline int statUniqueBarcodes() const {return m_dataset.statUniqueBarcodes();}
    inline int statUniqueGenes() const {return m_dataset.statUniqueGenes();}
    inline const QString& statTissue() const {return m_dataset.statTissue();}
    inline const QString& statSpecie() const {return m_dataset.statSpecie();}
    inline const QString& statCreated() const {return m_dataset.statCreated();}
    inline const QString& statComments() const {return m_dataset.statComments();}

    inline void id(const QString& id) { m_dataset.id(id); }
    inline void name(const QString& name) { m_dataset.name(name); }
    inline void chipId(const QString& chipId) { m_dataset.chipId(chipId); }
    inline void alignment(const QVariantList& alignment) { m_dataset.alignment(unserializeTransform(alignment)); }
    inline void figureBlue(const QString& figure_blue) { m_dataset.figureBlue(figure_blue); }
    inline void figureRed(const QString& figure_red) { m_dataset.figureRed(figure_red); }
    inline void figureStatus(int figure_status) { m_dataset.figureStatus(figure_status); }
    inline void statBarcodes(int barcodes) { m_dataset.statBarcodes(barcodes); }
    inline void statGenes(int genes) { m_dataset.statGenes(genes); }
    inline void statUniqueBarcodes(int unique_barcodes) { m_dataset.statUniqueBarcodes(unique_barcodes); }
    inline void statUniqueGenes(int unique_genes) { m_dataset.statUniqueGenes(unique_genes); }
    inline void statTissue(const QString& tissue) { m_dataset.statTissue(tissue); }
    inline void statSpecie(const QString& specie ) { m_dataset.statSpecie(specie); }
    inline void statCreated(const QString& created) { m_dataset.statCreated(created); }
    inline void statComments(const QString& comments) { m_dataset.statComments(comments); }

    // get parsed data model
    const Dataset& dataset() const { return m_dataset; }
    Dataset& dataset() { return m_dataset; }

private:
    
    const QTransform unserializeTransform(const QVariantList& transform) const;
    const QVariantList serializeTransform(const QTransform& transform) const;
    
    Dataset m_dataset;
};

#endif // DATASETDTO_H //
