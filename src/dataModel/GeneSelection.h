#ifndef GENESELECTION_H
#define GENESELECTION_H

#include <QString>

class GeneSelection
{
public:

    GeneSelection();
    GeneSelection(const QString name, qreal reads, qreal normalizedReads);
    explicit GeneSelection(const GeneSelection& other);
    virtual ~GeneSelection();

    GeneSelection& operator= (const GeneSelection& other);
    bool operator== (const GeneSelection& other) const;

    const QString& name() const { return m_name; }
    qreal reads() const { return m_reads; }
    qreal normalizedReads() const { return m_normalizedReads; }

    void name(const QString& name) { m_name = name; }
    void reads(qreal reads) { m_reads = reads; }
    void normalizedReads(qreal normalizedReads) { m_normalizedReads = normalizedReads;}

private:

    QString m_name;
    qreal m_reads;
    qreal m_normalizedReads;
};

#endif // GENESELECTION_H
