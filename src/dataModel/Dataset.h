#ifndef DATASET_H
#define DATASET_H

#include <QString>
#include <QTransform>

// Data model class to store datasets.
class Dataset
{

public:
    Dataset();
    explicit Dataset(const Dataset &other);
    ~Dataset();

    Dataset &operator=(const Dataset &other);
    bool operator==(const Dataset &other) const;
    // Id is the DataBase id
    const QString id() const;
    const QString name() const;
    // reference to image alignment object Id
    const QString imageAlignmentId() const;
    const QString statTissue() const;
    const QString statSpecies() const;
    const QString statComments() const;
    // the dataset is enabled or not
    bool enabled() const;
    // accounts that can access it
    const QVector<QString> grantedAccounts() const;
    const QString createdByAccount() const;
    const QString created() const;
    const QString lastModified() const;
    // the dataset has been downloaded from the cloud (true) or locally imported
    // (false)
    bool downloaded() const;

    void id(const QString &id);
    void name(const QString &name);
    void imageAlignmentId(const QString &alignmentId);
    void statTissue(const QString &statTissue);
    void statSpecies(const QString &statSpecies);
    void statComments(const QString &statComments);
    void enabled(const bool enabled);
    void grantedAccounts(const QVector<QString> &grantedAccounts);
    void createdByAccount(const QString &created);
    void created(const QString &created);
    void lastModified(const QString &lastModified);
    void downloaded(const bool downloaded);

private:
    QString m_id;
    QString m_name;
    QString m_alignmentId;
    QString m_statTissue;
    QString m_statSpecies;
    QString m_statComments;
    bool m_enabled;
    QVector<QString> m_grantedAccounts;
    QString m_createdByAccount;
    QString m_created;
    QString m_lastMofidied;
    bool m_downloaded;
};

#endif // DATASET_H
