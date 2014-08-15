/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef FEATURE_H
#define FEATURE_H

#include <QString>
#include <QColor>

// Data model class to store feature data.
class Feature
{

public:

    Feature();
    explicit Feature(const Feature &other);
    ~Feature();

    Feature& operator=(const Feature& other);
    bool operator==(const Feature& other) const;

    const QString id() const;
    const QString barcode() const;
    const QString gene() const;
    const QString annotation() const;
    int hits() const;
    double x() const;
    double y() const;

    void id(const QString& id);
    void barcode(const QString& barcode);
    void gene(const QString& gene);
    void annotation(const QString& annotation);
    void hits(int hits);
    void x(double x);
    void y(double y);

    //extended attribute to store the color
    const QColor color() const;
    void color(const QColor& color);

protected:

    QString m_id;
    QString m_barcode;
    QString m_gene;
    QString m_annotation;
    int m_hits;
    qreal m_x;
    qreal m_y;

    //extended
    QColor m_color;
};

#endif // FEATURE_H
