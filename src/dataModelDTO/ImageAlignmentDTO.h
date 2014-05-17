/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef IMAGEALIGNMENT_H
#define IMAGEALIGNMENT_H

#include <QObject>
#include <QString>

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

    explicit ImageAlignmentDTO(QObject* parent = 0);
    ~ImageAlignmentDTO();

    // binding
    void id(const QString& id) { m_imageAlignment.id(id); }
    void name(const QString& name) { m_imageAlignment.name(name); }
    void chipId(const QString& chipId) { m_imageAlignment.chipId(chipId); }
    void figureRed(const QString& figureRed) { m_imageAlignment.figureRed(figureRed); }
    void figureBlue(const QString& figureBlue) { m_imageAlignment.figureBlue(figureBlue); }
    void alignment(const QVariantList& alignment) { m_imageAlignment.alignment(alignment); }

    // get parsed data model
    const ImageAlignment& imageAlignment() const { return m_imageAlignment; }
    ImageAlignment& imageAlignment() { return m_imageAlignment; }

private:

    const QTransform unserializeTransform(const QVariantList& transform) const;
    const QVariantList serializeTransform(const QTransform& transform) const;

    ImageAlignment m_imageAlignment;
};

#endif // IMAGEALIGNMENT_H //
