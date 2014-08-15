#ifndef GENESELECTION_H
#define GENESELECTION_H

#include <QString>
#include <QVector>

//Selection Type is a container meant to be used
//to store aggregated selections
//TODO move to a separate file
class SelectionType {

public:

    SelectionType();
    SelectionType(const SelectionType& other);
    SelectionType(QString name, int reads, qreal
                  normalizedReads = 0, qreal pixeIntensity = 0, int count = 0);

    SelectionType& operator= (const SelectionType& other);
    SelectionType& operator+= (const SelectionType& other);
    bool operator< (const SelectionType& other) const;
    bool operator== (const SelectionType& other) const;

    QString name;
    int reads;
    //normalized reads not used at the moment
    //until we decide a final approach for the DEA
    //and the normalization
    qreal normalizedReads;
    qreal pixeIntensity;
    int count;
};

//Gene selection represents a selection of genes
//made by the user trough the UI.
//Gene selections are meant to be stored and shown
//in the experiments view
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
    const QString type() const;
    const QString status() const;
    const QVector<QString> oboFoundryTerms() const;
    const QString comment() const;
    bool enabled() const;
    const QString created() const;
    const QString lastModified() const;
    const QString datasetName() const;
    //return the total sum of reads in the Selection items
    int totalReads() const;

    void id(const QString& id);
    void name(const QString& name);
    void userId(const QString& userId);
    void datasetId(const QString& datasetId);
    void selectedItems(const selectedItemsList& selectedItems);
    void type(const QString& type);
    void status(const QString& status);
    void oboFoundryTerms(const QVector<QString>& oboFoundryTerms);
    void comment(const QString& comment);
    void enabled(const bool enabled);
    void created(const QString& created);
    void lastModified(const QString& lastModified);
    void datasetName(const QString& datasetName);

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
};

#endif // GENESELECTION_H
