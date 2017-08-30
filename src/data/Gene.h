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
    // true if the gene is selected
    bool selected() const;
    // the color of the gene
    const QColor color() const;
    // the threshold (reads)
    // the gene cut-off is used to discard genes whose count is below the cut off
    float cut_off() const;
    // the total number of transcripts for the gene in the dataset
    float totalCount() const;

    // Setters
    void name(const QString &name);
    void visible(const bool visible);
    void selected(const bool selected);
    void color(const QColor &color);
    void cut_off(const float cutoff);
    void totalCount(const float totalCount);

private:
    QString m_name;
    QColor m_color;
    bool m_visible;
    bool m_selected;
    float m_cutoff;
    float m_totalCount;
};

#endif // GENE_H //
