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
    virtual ~Feature();

    Feature& operator=(const Feature& other);
    bool operator==(const Feature& other) const;

    const QString id() const { return m_id; }
    const QString barcode() const { return m_barcode; }
    const QString gene() const { return m_gene; }
    const QString annotation() const {return m_annotation;}
    int hits() const { return m_hits; }
    double x() const { return m_x; }
    double y() const { return m_y; }

    void id(const QString& id) { m_id = id; }
    void barcode(const QString& barcode) { m_barcode = barcode; }
    void gene(const QString& gene) { m_gene = gene; }
    void annotation(const QString& annotation) {m_annotation = annotation; }
    void hits(int hits) { m_hits = hits; }
    void x(double x) { m_x = x; }
    void y(double y) { m_y = y; }

    //extended
    const QColor& color() const { return m_color; }
    void color(const QColor& color) { m_color = color; }
    bool selected() const {return m_selected;}
    void selected(bool selected) {m_selected = selected;}

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
    bool m_selected;
};

#endif // FEATURE_H
