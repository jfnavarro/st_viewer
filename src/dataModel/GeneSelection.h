#ifndef GENESELECTION_H
#define GENESELECTION_H

#include <QString>
#include <QVector>

//Selection Type is a container meant to be used
//to store aggregated selections
//TODO defined operator+  and operator<
class SelectionType {

public:

    SelectionType();
    SelectionType(QString name, int reads, qreal
                  normalizedReads = 0, qreal pixeIntensity = 0);

    SelectionType& operator= (const SelectionType& other);
    bool operator== (const SelectionType& other) const;

    QString name;
    int reads;
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
    bool enabled() const;
    const QString created() const;
    const QString lastModified() const;
    const QString datasetName() const;

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
    //added
    QString m_datasetName;
};

#endif // GENESELECTION_H
