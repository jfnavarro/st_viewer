/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef FEATURE_H
#define FEATURE_H
#include <QString>
#include "utils/Utils.h"
// Data model class to store feature data.
class Feature
{

public:

    Feature();
    explicit Feature(const Feature &other);
    virtual ~Feature();

    Feature& operator=(const Feature& other);

    bool operator==(const Feature& other) const;

    virtual const QString& id() const { return m_id; }
    virtual const QString& barcode() const { return m_barcode; }
    virtual const QString& gene() const { return m_gene; }
    virtual int hits() const { return m_hits; }
    virtual double x() const { return m_x; }
    virtual double y() const { return m_y; }

    virtual void id(const QString& id) { m_id = id; }
    virtual void barcode(const QString& barcode) { m_barcode = barcode; }
    virtual void gene(const QString& gene) { m_gene = gene; }
    virtual void hits(int hits) { m_hits = hits; }
    virtual void x(double x) { m_x = x; }
    virtual void y(double y) { m_y = y; }

protected:

    QString m_id;
    QString m_barcode;
    QString m_gene;
    int m_hits;
    double m_x;
    double m_y;
};

#endif // FEATURE_H
