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

// this class contains the gene plotter visual
// data containers and it gives and an easy interface
// to add/remove/update data
// OpenGL buffers are configured automatically
// the principle is that the data points are stored
// as triangles but they are quads
class GeneData : public QGeometryData
{

public:

    //ref count represents how many times a
    //certain position(feature) is being shown
    typedef QArray<qreal> refCountType;

    GeneData();
    virtual ~GeneData();

    void clearData();

    //adds a new geometry to the arrays
    int addQuad(qreal x, qreal y, qreal size = 1.0, QColor4ub color = Qt::white);

    //update geometry
    void updateQuadSize(const int index, qreal x, qreal y, qreal size);
    void updateQuadColor(const int index, QColor4ub newcolor);
    void updateQuadSelected(const int index, bool selected);
    void updateQuadRefCount(const int index, float refcount);
    void updateQuadValue(const int index, float value);

    //some getters
    QColor4ub quadColor(const int index) const;
    bool quadSelected(const int index) const;
    float quadRefCount(const int index) const;
    float quadValue(const int index) const;

    //reset functions for custom arrays
    void resetRefCount();
    void resetValues();
    void resetSelection(bool selected);

private:

    GeneData::refCountType m_refCount;

    Q_DISABLE_COPY(GeneData)
};

#endif // GENEDATA_H
