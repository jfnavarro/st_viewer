/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GENEPLOTTERGL_H
#define GENEPLOTTERGL_H

#include <QGraphicsItem>
#include <QScopedPointer>
#include "utils/Utils.h"
#include "data/DataProxy.h"

class ColorScheme;
class QGLShaderProgram;
class GeneRendererGL;
class GridRendererGL;
class SelectionEvent;
class QWidget;

// Specialized graphical object class used to visualize gene data in the
// cell view. Contains all necessary data to configure and display genes
// as well as the grid on which they exist.
class GenePlotterGL: public QGraphicsObject
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:

    explicit GenePlotterGL(QGraphicsItem* parent = 0);
    virtual ~GenePlotterGL();

    // clear rendering data
    void clear();
    
    // reset member variables
    void reset();

    // init opengl data
    void initGL();
    
    // load rendering shader program
    void loadShader(const uint id, const QString &vertexPath, const QString &fragmentPath);

    //some overloaded methods
    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

    // creates the gene and chip data
    void updateGeneData();
    
    // update chip size and create the chip data
    void updateChipSize();
    
    // load dataset trandformation matrix
    void updateTransformation();

    // return the selected features
    DataProxy::FeatureListPtr getSelectedFeatures();

public slots:

    // update selection
    void setSelectionArea(const SelectionEvent*);

    // clear the current selection
    void clearSelectionArea();

    // update gene render list
    void updateGeneColor(DataProxy::GenePtr);
    void updateGeneSelection(DataProxy::GenePtr);

    // select by genes
    void selectGenes(const DataProxy::GeneList&);

    // some setters
    void setGeneVisible(bool geneVisible);
    void setGeneShape(const Globals::Shape&);
    void setGeneLowerLimit(int geneLimit);
    void setGeneUpperLimit(int geneLimit);
    void setHitCountLimits(int min, int max, int sum);
    void setGeneIntensity(qreal geneIntensity);
    void setGeneSize(qreal geneSize);
    void setGeneVisualMode(const Globals::VisualMode&);
    void setGeneThresholdMode(const Globals::ThresholdMode&);
    void setGridVisible(bool gridVisible);
    void setGridColor(const QColor& gridColor);

signals:
    //signal emitted every time we make a selection
    void featuresSelected(DataProxy::FeatureListPtr);

private:
    // grid attributes
    bool m_gridVisible;

    // shaders
    typedef QMap<uint, QGLShaderProgram *> ShaderProgramList;
    ShaderProgramList m_geneProgramList;
    QGLShaderProgram *m_geneProgram;

    // gene attributes
    Globals::Shape m_geneShape;
    bool m_geneVisible;

    //classes that encapsulates all the rendering
    QScopedPointer<GeneRendererGL> m_geneRenderer;
    QScopedPointer<GridRendererGL> m_chipRenderer;
};

#endif // GENEPLOTTERGL_H
