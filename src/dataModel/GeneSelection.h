#ifndef GENESELECTION_H
#define GENESELECTION_H

#include <QString>
#include <QVector>

// Selection Type is a container meant to be used
// to store aggregated selections
//TODO move to a separate file
class SelectionType {

public:

    SelectionType();
    SelectionType(const SelectionType& other);
    SelectionType(QString name, int reads, int
                  normalizedReads, int count = 0);

    SelectionType& operator= (const SelectionType& other);
    bool operator< (const SelectionType& other) const;
    bool operator== (const SelectionType& other) const;

    QString name;
    int reads;
    int normalizedReads;
    int count;
};

// Gene selection represents a selection of genes
// made by the user trough the UI, user selects barcodes/spots and then the information
// is aggregated by genes.
// Gene selections are meant to be stored and shown
// in the experiments view
class GeneSelection
{

public:

    typedef QList<SelectionType> selectedItemsList;

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
    selectedItemsList selectedItems();
    const QString status() const;
    const QVector<QString> oboFoundryTerms() const;
    const QString comment() const;
    bool enabled() const;
    const QString created() const;
    const QString lastModified() const;
    const QString datasetName() const;
    const QString type() const;
    const QByteArray tissueSnapShot() const;

    // returns the total sum of reads in the Selection items
    int totalReads() const;
    // returns the total number of features in this selection
    int totalFeatures() const;

    void id(const QString& id);
    void name(const QString& name);
    void userId(const QString& userId);
    void datasetId(const QString& datasetId);
    void selectedItems(const selectedItemsList& selectedItems);
    void status(const QString& status);
    void oboFoundryTerms(const QVector<QString>& oboFoundryTerms);
    void comment(const QString& comment);
    void enabled(const bool enabled);
    void created(const QString& created);
    void lastModified(const QString& lastModified);
    void datasetName(const QString& datasetName);
    void type(const QString& type);
    void tissueSnapShot(const QByteArray &tissueSnapShot);

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
    bool m_enabled;
    QString m_created;
    QString m_lastMofidied;
    QString m_datasetName;
    QByteArray m_tissueSnapShot;

    // for caching purposes
    int m_totalReads;
    int m_totalFeatures;
};

#endif // GENESELECTION_H
