/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef FEATURE_H
#define FEATURE_H

#include <QString>
#include <QColor>
#include <QSharedPointer>

#include "Gene.h"

// Data model class to store feature data
// A feature corresponds to a tuple (barcode or spot in the array)
// and a gene. In each barcode/spot there can be up to 20k genes.
// The coordinates x,y refers to chip coordinates but the chip object
// contains an affine matrix that converts chip coordinates to image pixel coordinates
// which is what we eventually visualize in the cell view.
class Feature
{

public:
    // TODO duplicated in DataProxy
    typedef QSharedPointer<Gene> GenePtr;

    Feature();
    explicit Feature(const Feature& other);
    Feature(QString barcode, QString gene, double x, double y, int hits);
    ~Feature();

    Feature& operator=(const Feature& other);
    bool operator==(const Feature& other) const;

    // barcode corresponds to the DNA ID of the spot
    const QString barcode() const;
    // the name of the gene
    const QString gene() const;
    const QString annotation() const;
    // the number of reads
    unsigned hits() const;
    // the coordinates of the spot
    double x() const;
    double y() const;

    void barcode(const QString& barcode);
    void gene(const QString& gene);
    void annotation(const QString& annotation);
    void hits(unsigned hits);
    void x(double x);
    void y(double y);

    // Reference to the Gene object just for convenience
    // this reference will be instantiated when the features are parsed
    GenePtr geneObject() const;
    void geneObject(GenePtr gene);

protected:
    QString m_barcode;
    QString m_gene;
    QString m_annotation;
    unsigned m_hits;
    double m_x;
    double m_y;

    // extended attribute
    GenePtr m_geneObject;
};

#endif // FEATURE_H
