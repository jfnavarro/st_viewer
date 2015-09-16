#ifndef IMAGEALIGNMENT_H
#define IMAGEALIGNMENT_H

#include <QString>
#include <QTransform>

// Image alignment represents the mapping of the features data
// to the images of the tissue.

class ImageAlignment
{
public:
    ImageAlignment();
    explicit ImageAlignment(const ImageAlignment& other);
    ~ImageAlignment();

    ImageAlignment& operator=(const ImageAlignment& other);
    bool operator==(const ImageAlignment& other) const;

    const QString id();
    const QString name() const;
    const QString chipId() const;
    const QString figureRed() const;
    const QString figureBlue() const;
    const QTransform alignment() const;
    const QString created() const;
    const QString lastModified() const;

    void id(const QString& id);
    void name(const QString& name);
    void chipId(const QString& chipId);
    void figureRed(const QString& figureRed);
    void figureBlue(const QString& figureBlue);
    void alignment(const QTransform& alignment);
    void created(const QString& created);
    void lastModified(const QString& lastModified);

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
