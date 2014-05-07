/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef GENEDATA_H
#define GENEDATA_H

#include <QGeometryData>
#include <QArray>

class QColor4ub;
class QGLAttributeValue;
class QCustomDataArray;

/* this class contains the gene plotter visual
 * data containers and it gives and an easy interface
 * to add/remove/update data
 * OpenGL buffers are configured automatically
 * the principle is that the data points are stored
 * as triangles but they are quads
 */
class GeneData : public QGeometryData
{

public:

    typedef QArray<GLfloat> refCountType;

    GeneData();
    virtual ~GeneData();

    void clearData();

    int addQuad(qreal x, qreal y, qreal size = 1.0f, QColor4ub color = Qt::white);

    void updateQuadSize(const int index, qreal x, qreal y, qreal size);
    void updateQuadColor(const int index, QColor4ub newcolor);
    void updateQuadVisible(const int index, bool visible);
    void updateQuadSelected(const int index, bool selected);
    void updateQuadRefCount(const int index, qreal refcount);
    void updateQuadValue(const int index, qreal value);

    QColor4ub quadColor(const int index) const;
    bool quadVisible(const int index) const;
    bool quadSelected(const int index) const;
    qreal quadRefCount(const int index) const;
    qreal quadValue(const int index) const;

    void resetRefCount();
    void resetValues();
    void resetSelection(bool state);

private:

    GeneData::refCountType m_refCount;

    Q_DISABLE_COPY(GeneData)
};

#endif // GENEDATA_H
