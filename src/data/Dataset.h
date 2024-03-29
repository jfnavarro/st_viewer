#ifndef DATASET_H
#define DATASET_H

#include <QString>
#include <QTransform>
#include <QSharedPointer>

class STData;
class DatasetImporter;

// Data model class to store datasets.
// A dataset is composed of a data frame (matrix of counts
// with spots and genes), an image of the tissue and a coordinates
// to pixel mapping file. Optionally users may load a 3D Mesh too.
// Datasets can be 2D or 3D.
// The alignment matrix to map spot coordiantes to pixel coordinates
// and the tiles of the HE image are generated when the dataset is loaded.
class Dataset
{

public:

    Dataset();
    explicit Dataset(const DatasetImporter &importer);
    explicit Dataset(const Dataset &other);
    ~Dataset();

    Dataset &operator=(const Dataset &other);
    bool operator==(const Dataset &other) const;

    // the reference to the ST Data
    const QSharedPointer<STData> data() const;

    // getters
    const QString &name() const;
    const QString &dataFile() const;
    const QString &imageFile() const;
    const QString &meshFile() const;
    const QString &spotsFile() const;
    const QString &statComments() const;
    double scalingFactor() const;

    // generated
    const QTransform &alignmentMatrix() const;
    const QVector<QPair<QImage, QPoint>> &image_tiles() const;
    const QRect image_bounds() const;
    bool is3D() const;

    // setters
    void name(const QString &name);
    void dataFile(const QString &datafile);
    void imageFile(const QString &image_file);
    void meshFile(const QString &mesh_file);
    void spotsFile(const QString &spots_file);
    void statComments(const QString &statComments);
    void scalingFactor(const double scaling_factor);

    // creates the STData object (parse data)
    // Parses : matrix of counts, image and spots-file
    // throws exception if parsing is something went wrong
    void load_data();

private:

    // Function to parse the image and break into tiles
    bool load_Image();

    QString m_name;
    QString m_statComments;
    QString m_data_file;
    QString m_image_file;
    QString m_mesh_file;
    QString m_spots_file;
    double m_scaling_factor;
    bool m_is3D;

    // generated
    QTransform m_alignment;
    QVector<QPair<QImage, QPoint>> m_image_tiles;
    QRect m_image_bounds;

    // ST data
    QSharedPointer<STData> m_data;
};

#endif // DATASET_H
