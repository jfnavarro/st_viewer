#ifndef GENE_H
#define GENE_H

#include <QString>
#include <QColor>

// Data model class to store gene data.
// The genes are part of the ST Data, they are modeled in a class
// to encapculate their status and attributes
class Gene
{

public:

    Gene();
    explicit Gene(const QString &name);
    explicit Gene(const Gene &other);
    ~Gene();

    Gene &operator=(const Gene &other);
    bool operator==(const Gene &other) const;

    // the name of the gene
    const QString name() const;
    // true if gene is visible
    bool visible() const;
    // the color of the gene
    const QColor color() const;
    // the total number of transcripts for the gene in the dataset
    double totalCount() const;

    // Setters
    void name(const QString &name);
    void visible(const bool visible);
    void color(const QColor &color);
    void totalCount(const double totalCount);

private:
    QString m_name;
    QColor m_color;
    bool m_visible;
    double m_totalCount;
};

#endif // GENE_H //
