#ifndef DATASET_H
#define DATASET_H

#include <QString>
#include <QTransform>
#include <memory>

class STData;

// Data model class to store datasets.
class Dataset
{

public:
    Dataset();
    explicit Dataset(const Dataset &other);
    ~Dataset();

    Dataset &operator=(const Dataset &other);
    bool operator==(const Dataset &other) const;
    // the reference to the ST Data matrix
    const std::shared_ptr<STData> data() const;
    // the name of the dataset
    const QString name() const;
    // 3x3 Affine transformation matrix
    const QTransform imageAlignment() const;
    // Some useful stats
    const QString statTissue() const;
    const QString statSpecies() const;
    const QString statComments() const;
    // Setters
    void data(const STData &data);
    void name(const QString &name);
    void imageAlignment(const QTransform &alignmentId);
    void statTissue(const QString &statTissue);
    void statSpecies(const QString &statSpecies);
    void statComments(const QString &statComments);

private:
    QString m_name;
    QTransform m_alignment;
    QString m_statTissue;
    QString m_statSpecies;
    QString m_statComments;
    std::shared_ptr<STData> m_data;
};

#endif // DATASET_H
