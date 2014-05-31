#ifndef GENESELECTION_H
#define GENESELECTION_H

#include <QString>
#include <QVector>

class GeneSelection
{

public:

    //TODO move definition to cpp
    struct SelectionType {
        SelectionType() : name(), reads(0), normalizedReads(0), pixeIntensity(0) {}
        SelectionType(QString name, qreal reads,
                      qreal normalizedReads = 0, qreal pixeIntensity = 0)
            : name(name), reads(reads), normalizedReads(normalizedReads), pixeIntensity(pixeIntensity) {}
        SelectionType& operator= (const SelectionType& other)
        {
            name = other.name;
            reads = other.reads;
            normalizedReads = other.normalizedReads;
            pixeIntensity = other.pixeIntensity;
            return (*this);
        }

        bool operator== (const SelectionType& other) const
        {
            return( name == other.name &&
                      reads == other.reads &&
                      normalizedReads == other.normalizedReads &&
                      pixeIntensity == other.pixeIntensity
                );
        }
        QString name;
        qreal reads;
        qreal normalizedReads;
        qreal pixeIntensity;
    };

    typedef QVector<SelectionType> selectedItemsList;

    GeneSelection();
    explicit GeneSelection(const GeneSelection& other);
    ~GeneSelection();

    GeneSelection& operator= (const GeneSelection& other);
    bool operator== (const GeneSelection& other) const;

    const QString id() const;
    const QString name() const;
    const QString userId() const;
    const QString datasetId() const;
    const selectedItemsList selectedItems() const;
    const QString type() const;
    const QString status() const;
    const QVector<QString> oboFoundryTerms() const;
    const QString comment() const;
    const QString resultFile() const;

    void id(const QString& id);
    void name(const QString& name);
    void userId(const QString& userId);
    void datasetId(const QString& datasetId);
    void selectedItems(const selectedItemsList& selectedItems);
    void type(const QString& type);
    void status(const QString& status);
    void oboFoundryTerms(const QVector<QString>& oboFoundryTerms);
    void comment(const QString& comment);
    void resultFile(const QString& file);

    //auxiliary function to get the unique seletion items (aka unique names with summed
    //reads
    static const selectedItemsList getUniqueSelectedItems(
            const selectedItemsList& selectedItems);

private:

    QString m_id;
    QString m_name;
    QString m_userId;
    QString m_datasetId;
    selectedItemsList m_selectedItems;
    QString m_type;
    QString m_status;
    QVector<QString> m_oboFroundryTerms;
    QString m_comment;
    QString m_resultFile;
};

#endif // GENESELECTION_H
