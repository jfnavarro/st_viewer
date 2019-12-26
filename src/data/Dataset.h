#ifndef DATASET_H
#define DATASET_H

#include <QString>
#include <QTransform>
#include <QSharedPointer>

class STData;
class DatasetImporter;

// Data model class to store datasets.
class Dataset
{

public:
    Dataset();
    explicit Dataset(const DatasetImporter &importer);
    explicit Dataset(const Dataset &other);
    ~Dataset();

    Dataset &operator=(const Dataset &other);
    bool operator==(const Dataset &other) const;

    // The reference to the ST Data
    const QSharedPointer<STData> data() const;

    // Getters
    const QString name() const;
    const QString dataFile() const;
    const QTransform imageAlignment() const;
    const QString imageAlignmentFile() const;
    const QString imageFile() const;
    const QString spotsFile() const;
    const QString statTissue() const;
    const QString statSpecies() const;
    const QString statComments() const;
    const QPoint xrange() const;
    const QPoint yrange() const;
    const QPoint zrange() const;

    // Setters
    void name(const QString &name);
    void dataFile(const QString &datafile);
    void imageAlignment(const QTransform &alignment);
    void imageAlignmentFile(const QString &aligment_file);
    void imageFile(const QString &image_file);
    void spotsFile(const QString &spots_file);
    void statTissue(const QString &statTissue);
    void statSpecies(const QString &statSpecies);
    void statComments(const QString &statComments);
    void xrange(const QPoint &chip);
    void yrange(const QPoint &chip);
    void zrange(const QPoint &chip);

    // creates the STData object (parse data)
    // Parses : matrix of counts, image, size factors (if any), alignment (if any),
    //          spots-file (if any)
    // throws exception if parsing is something went wrong
    void load_data();

private:

    // Function to load the image aligment matrix from a file
    bool load_imageAligment();

    QString m_name;
    QString m_statTissue;
    QString m_statSpecies;
    QString m_statComments;
    QString m_data_file;
    QString m_image_file;
    QString m_alignment_file;
    QString m_spots_file;
    QPoint m_xrange;
    QPoint m_yrange;
    QPoint m_zrange;
    bool m_is3D;

    // generated
    QTransform m_alignment;
    QSharedPointer<STData> m_data;
    QVector<QImage> m_image_tiles;
};

#endif // DATASET_H
