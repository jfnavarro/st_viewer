/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GENE_H
#define GENE_H


#include <QString>
#include <QColor>

#include "utils/Utils.h"

// Data model class to store gene data.
class Gene
{

public:

    Gene();
    explicit Gene(const Gene& other);
    explicit Gene(const QString& id, const QString& name, bool m_hidden = false,
                  QColor m_color = Globals::color_gene);

    virtual ~Gene();

    Gene& operator= (const Gene& other);
    bool operator== (const Gene& other) const;

    const QString& id() const { return m_id; }
    const QString& name() const { return m_name; }
    bool selected() const { return m_selected; }
    const QColor& color() const { return m_color; }

    void id(const QString& id) { m_id = id; }
    void name(const QString& name) { m_name = name; }
    void selected(bool selected) { m_selected = selected; }
    void color(const QColor& color) { m_color = color;}

    //NOTE ambiguous property shouldn't be stored as part of the name
    bool isAmbiguous() const;

private:

    QString m_id;
    QString m_name;
    QColor m_color;
    bool m_selected;
};

#endif // GENE_H //
