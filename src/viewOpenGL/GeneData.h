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

//TODO selected and value arrays should be INT (next Qt3D API will allow that)
class GeneData : public QGeometryData
{

public:

    GeneData();
    virtual ~GeneData();

    //clear data and geometry arrays
    void clearData();

    //adds a new geometry to the arrays
    int addQuad(const float x,
                const float y,
                const float size = 1.0,
                const QColor4ub color = Qt::white);

    //update geometry and rendering data
    void updateQuadSize(const int index,
                        const qreal x,
                        const qreal y,
                        const qreal size);
    void updateQuadColor(const int index, const QColor4ub newcolor);
    void updateQuadSelected(const int index, const bool selected);
    void updateQuadValue(const int index, const float value);

    //some getters
    QColor4ub quadColor(const int index) const;
    bool quadSelected(const int index) const;
    float quadValue(const int index) const;

    //reset functions for custom arrays
    void resetValues();
    void resetSelection(const bool selected);

    //is this gene data valid to be rendered?
    bool isValid() const;

    Q_DISABLE_COPY(GeneData)
};

#endif // GENEDATA_H
