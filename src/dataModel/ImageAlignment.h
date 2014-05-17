#ifndef IMAGEALIGNMENT_H
#define IMAGEALIGNMENT_H

#include <QString>
#include <QTransform>

class ImageAlignment
{
public:

    ImageAlignment();
    explicit ImageAlignment(const ImageAlignment& other);
    virtual ~ImageAlignment();

    ImageAlignment& operator= (const ImageAlignment& other);
    bool operator== (const ImageAlignment& other) const;

    const QString id() { return m_id; }
    const QString name() const { return m_name; }
    const QString chipId() const { return m_chipId; }
    const QString figureRed() const { return m_figureRed; }
    const QString figureBlue() const { return m_figureBlue; }
    const QTransform alignment() const { return m_alignment; }

    void id(const QString& id) { m_id = id; }
    void name(const QString& name) { m_name = name; }
    void chipId(const QString& chipId) { m_chipId = chipId; }
    void figureRed(const QString& figureRed) { m_figureRed = figureRed; }
    void figureBlue(const QString& figureBlue) { m_figureBlue = figureBlue; }
    void alignment(const QTransform& alignment) { m_alignment = alignment; }

private:

    QString m_id;
    QString m_name;
    QString m_chipId;
    QString m_figureRed;
    QString m_figureBlue;
    QTransform m_alignment;
};

#endif // IMAGEALIGNMENT_H
