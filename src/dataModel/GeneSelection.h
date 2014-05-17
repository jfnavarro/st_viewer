#ifndef GENESELECTION_H
#define GENESELECTION_H

#include <QString>
#include <QStringList>

class GeneSelection
{
public:

    GeneSelection();
    explicit GeneSelection(const GeneSelection& other);
    ~GeneSelection();

    GeneSelection& operator= (const GeneSelection& other);
    bool operator== (const GeneSelection& other) const;

    const QString id() const { return m_id; }
    const QString name() const { return m_name; }
    const QString userId() const { return m_userId; }
    const QString datasetId() const { return m_datasetId; }
    const QStringList featureIds() const { return m_featureIds; }
    const QString type() const { return m_type; }
    const QString status() const { return m_status; }
    const QStringList oboFoundryTerms() const { return m_oboFroundryTerms; }
    const QString comment() const { return m_comment; }

    void id(const QString& id) { m_id = id; }
    void name(const QString& name) { m_name = name; }
    void userId(const QString& userId) { m_userId = userId; }
    void datasetId(const QString& datasetId) { m_datasetId = datasetId; }
    void featureIds(const QStringList& featureIds) { m_featureIds = featureIds; }
    void type(const QString& type) { m_type = type; }
    void status(const QString& status) { m_status = status; }
    void oboFoundryTerms(const QStringList& oboFoundryTerms)
      { m_oboFroundryTerms = oboFoundryTerms; }
    void comment(const QString& comment) { m_comment = comment; }

private:

    QString m_id;
    QString m_name;
    QString m_userId;
    QString m_datasetId;
    QStringList m_featureIds;
    QString m_type;
    QString m_status;
    QStringList m_oboFroundryTerms;
    QString m_comment;
};

#endif // GENESELECTION_H
