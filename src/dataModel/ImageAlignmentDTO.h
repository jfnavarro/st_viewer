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

// TODO move definitions to CPP and/or consider removing DTOs
class ImageAlignmentDTO : public QObject
{

    Q_OBJECT

    Q_PROPERTY(QString id READ id WRITE id)
    Q_PROPERTY(QString name READ name WRITE name)
    Q_PROPERTY(QString chip_id READ chipId WRITE chipId)
    Q_PROPERTY(QString figure_red READ figureRed WRITE figureRed)
    Q_PROPERTY(QString figure_blue READ figureBlue WRITE figureBlue)
    Q_PROPERTY(QVariantList alignment_matrix READ alignment WRITE alignment)
    Q_PROPERTY(QString created_at READ created WRITE created)
    Q_PROPERTY(QString last_modified READ lastModified WRITE lastModified)

public:
    explicit ImageAlignmentDTO(QObject* parent = 0)
        : QObject(parent)
    {
    }
    ~ImageAlignmentDTO() {}

    // binding
    void id(const QString& id) { m_imageAlignment.id(id); }
    void name(const QString& name) { m_imageAlignment.name(name); }
    void chipId(const QString& chipId) { m_imageAlignment.chipId(chipId); }
    void figureRed(const QString& figureRed) { m_imageAlignment.figureRed(figureRed); }
    void figureBlue(const QString& figureBlue) { m_imageAlignment.figureBlue(figureBlue); }
    void alignment(const QVariantList& alignment)
    {
        m_imageAlignment.alignment(unserializeTransform(alignment));
    }
    void created(const QString& created) { m_imageAlignment.created(created); }
    void lastModified(const QString& lastModified) { m_imageAlignment.lastModified(lastModified); }

    // read
    const QString id() { return m_imageAlignment.id(); }
    const QString name() { return m_imageAlignment.name(); }
    const QString chipId() { return m_imageAlignment.chipId(); }
    const QString figureRed() { return m_imageAlignment.figureRed(); }
    const QString figureBlue() { return m_imageAlignment.figureBlue(); }
    const QVariantList alignment() { return serializeTransform(m_imageAlignment.alignment()); }
    const QString created() const { return m_imageAlignment.created(); }
    const QString lastModified() const { return m_imageAlignment.lastModified(); }

    // get parsed data model
    const ImageAlignment& imageAlignment() const { return m_imageAlignment; }
    ImageAlignment& imageAlignment() { return m_imageAlignment; }

private:
    const QVariantList serializeTransform(const QTransform& transform) const
    {
        QVariantList serializedTransform;
        // serialize data
        serializedTransform << QVariant::fromValue(transform.m11())
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
        QVariantList::const_iterator it;
        QVariantList::const_iterator end = serializedTransform.end();
        for (it = serializedTransform.begin(); it != end; ++it) {
            values << it->value<qreal>();
        }

        // parse transform matrix
        Q_ASSERT_X(values.size() == 9,
                   "ImageAlignmentDTO",
                   "Unable to unserialize transform matrix!");
        transform = QTransform(values[0],
                               values[3],
                               values[6],
                               values[1],
                               values[4],
                               values[7],
                               values[2],
                               values[5],
                               values[8]);

        return transform;
    }

    ImageAlignment m_imageAlignment;
};

#endif // IMAGEALIGNMENTDTO_H //
