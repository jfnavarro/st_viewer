/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef IMAGEALIGNMENTDTO_H
#define IMAGEALIGNMENTDTO_H

#include <QObject>
#include <QString>
#include <QVariantList>

#include "dataModel/ImageAlignment.h"

// DTOs (Data transform objects) provides the bridge between the client
// specific data model objects and the server data. This is accomplished by
// defining dynamic properties that enable automated serialization and
// deserialization of server data.

class ImageAlignmentDTO : public QObject
{

    Q_OBJECT

    Q_PROPERTY(QString id WRITE id)
    Q_PROPERTY(QString name WRITE name)
    Q_PROPERTY(QString chip_id WRITE chipId)
    Q_PROPERTY(QString figure_red WRITE figureRed)
    Q_PROPERTY(QString figure_blue WRITE figureBlue)
    Q_PROPERTY(QVariantList alignment_matrix WRITE alignment)

public:

    explicit ImageAlignmentDTO(QObject* parent = 0) : QObject(parent) { }
    ~ImageAlignmentDTO() { }

    // binding
    void id(const QString& id) { m_imageAlignment.id(id); }
    void name(const QString& name) { m_imageAlignment.name(name); }
    void chipId(const QString& chipId) { m_imageAlignment.chipId(chipId); }
    void figureRed(const QString& figureRed) { m_imageAlignment.figureRed(figureRed); }
    void figureBlue(const QString& figureBlue) { m_imageAlignment.figureBlue(figureBlue); }
    void alignment(const QVariantList& alignment)
      { m_imageAlignment.alignment(unserializeTransform(alignment)); }

    // get parsed data model
    const ImageAlignment& imageAlignment() const { return m_imageAlignment; }
    ImageAlignment& imageAlignment() { return m_imageAlignment; }

private:

    const QVariantList serializeTransform(const QTransform& transform) const
    {
        QVariantList serializedTransform;
        // serialize data
        serializedTransform
                << QVariant::fromValue(transform.m11())
                << QVariant::fromValue(transform.m12())
                << QVariant::fromValue(transform.m13())
                << QVariant::fromValue(transform.m21())
                << QVariant::fromValue(transform.m22())
                << QVariant::fromValue(transform.m23())
                << QVariant::fromValue(transform.m31())
                << QVariant::fromValue(transform.m32())
                << QVariant::fromValue(transform.m33());
        return serializedTransform;
    }

    const QTransform unserializeTransform(const QVariantList& serializedTransform) const
    {
        QTransform transform(Qt::Uninitialized);
        // unserialize data
        QVector<qreal> values;
        QVariantList::const_iterator it, end = serializedTransform.end();
        for (it = serializedTransform.begin(); it != end; ++it) {
            values << it->value<qreal>();
        }
        // parse transform matrix
        QVector<qreal>::size_type size = values.size();
        switch (size) {
        // 3x2 afinite transform matrix
        case 6:
            transform = QTransform(
                            values[0], values[3],
                            values[1], values[4],
                            values[2], values[5]
                        );
            break;
        // normal 3x3 transform matrix
        case 9:
            transform = QTransform(
                            values[0], values[3], values[6],
                            values[1], values[4], values[7],
                            values[2], values[5], values[8]
                        );
            break;
        // meh...
        default:
            qDebug() << "[DatasetDTO] Warning: Unable to"
                        " unserialize transform matrix!" << endl << serializedTransform;
            transform = QTransform();
            break;
        }

        return transform;
    }

    ImageAlignment m_imageAlignment;
};

#endif // IMAGEALIGNMENTDTO_H //
