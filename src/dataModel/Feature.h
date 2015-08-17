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
// A feature correspond to a tuple (barcode or spot in the array)
// and gene. In each barcode/spot there can be up to 20k genes.
class Feature
{

public:

    // TODO duplicated in DataProxy
    typedef QSharedPointer<Gene> GenePtr;

    Feature();
    explicit Feature(const Feature &other);
    Feature(QString barcode, QString gene, double x, double y, int hits);
    ~Feature();

    Feature& operator=(const Feature& other);
    bool operator==(const Feature& other) const;

    const QString barcode() const;
    const QString gene() const;
    const QString annotation() const;
    int hits() const;
    double x() const;
    double y() const;

    void barcode(const QString& barcode);
    void gene(const QString& gene);
    void annotation(const QString& annotation);
    void hits(int hits);
    void x(double x);
    void y(double y);

    // reference to the Gene object just for convenience
    GenePtr geneObject() const;
    void geneObject(GenePtr gene);

protected:

    QString m_barcode;
    QString m_gene;
    QString m_annotation;
    int m_hits;
    double m_x;
    double m_y;

    // extended attribute
    GenePtr m_geneObject;
};

#endif // FEATURE_H
