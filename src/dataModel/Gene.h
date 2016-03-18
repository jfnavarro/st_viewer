#ifndef GENE_H
#define GENE_H

#include <QString>
#include <QColor>

#include "utils/Utils.h"

// Data model class to store gene data.
// The genes are part of the features, they are modeled in a class
// to encapculate their status and attributes
class Gene
{

public:
    Gene();
    explicit Gene(const Gene& other);
    explicit Gene(const QString& name,
                  bool selected = false,
                  QColor m_color = Globals::DEFAULT_COLOR_GENE,
                  const unsigned cutoff = 1);
    ~Gene();

    Gene& operator=(const Gene& other);
    bool operator==(const Gene& other) const;

    const QString name() const;
    bool selected() const;
    const QColor color() const;
    unsigned cut_off() const;

    void name(const QString& name);
    void selected(bool selected);
    void color(const QColor& color);
    void cut_off(const unsigned cutoff);

    // NOTE ambiguous property shouldn't be stored as part of the name
    bool isAmbiguous() const;

private:
    QString m_name;
    QColor m_color;
    bool m_selected;
    unsigned m_cutoff;
};

#endif // GENE_H //
