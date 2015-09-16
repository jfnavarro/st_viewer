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
    explicit Gene(const QString& name,
                  bool selected = false,
                  QColor m_color = Globals::DEFAULT_COLOR_GENE);
    ~Gene();

    Gene& operator=(const Gene& other);
    bool operator==(const Gene& other) const;

    const QString name() const;
    bool selected() const;
    const QColor color() const;

    void name(const QString& name);
    void selected(bool selected);
    void color(const QColor& color);

    // NOTE ambiguous property shouldn't be stored as part of the name
    bool isAmbiguous() const;

private:
    QString m_name;
    QColor m_color;
    bool m_selected;
};

#endif // GENE_H //
