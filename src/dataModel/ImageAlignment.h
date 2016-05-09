#ifndef IMAGEALIGNMENT_H
#define IMAGEALIGNMENT_H

#include <QString>
#include <QTransform>

// Image alignment represents the mapping of the features data
// to the images of the tissue.
// It contains the url of the images (read and blue)
// a reference to the chip object and an alignment matrix
// to convert the features to the image coordinate system.
// blue and read refers to the image used to visualize(blue)
// and the image used to align(red). The red image
// will not available for normal users.

class ImageAlignment
{
public:
    ImageAlignment();
    explicit ImageAlignment(const ImageAlignment &other);
    ~ImageAlignment();

    ImageAlignment &operator=(const ImageAlignment &other);
    bool operator==(const ImageAlignment &other) const;

    // ID corresponds to the database Id
    const QString id();
    const QString name() const;
    // ID corresponds to the database Id of the chip
    const QString chipId() const;
    const QString figureRed() const;
    const QString figureBlue() const;
    // 3x3 affine matrix to transform array coordinates to image coordinates
    const QTransform alignment() const;
    const QString created() const;
    const QString lastModified() const;

    void id(const QString &id);
    void name(const QString &name);
    void chipId(const QString &chipId);
    void figureRed(const QString &figureRed);
    void figureBlue(const QString &figureBlue);
    void alignment(const QTransform &alignment);
    void created(const QString &created);
    void lastModified(const QString &lastModified);

private:
    QString m_id;
    QString m_name;
    QString m_chipId;
    QString m_figureRed;
    QString m_figureBlue;
    QTransform m_alignment;
    QString m_created;
    QString m_lastModified;
};

#endif // IMAGEALIGNMENT_H
