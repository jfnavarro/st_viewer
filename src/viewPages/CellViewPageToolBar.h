/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef CELLVIEWPAGETOOLBAR_H
#define CELLVIEWPAGETOOLBAR_H

#include <QToolBar>

#include "utils/Utils.h"

class QMenuBar;
class QMenu;
class QActionGroup;
class QAction;
class QWidgetAction;
class QToolBar;


class SpinBoxSlider;
class QSlider;
class QComboBox;

class CellViewPageToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit CellViewPageToolBar(QWidget *parent = 0);

    // actions for toolbar

    QAction *m_actionNavigate_goBack = nullptr;
    QAction *m_actionSave_save = nullptr;
    QAction *m_actionSave_print = nullptr;
    QAction *m_actionSelection_showSelectionDialog = nullptr;

    QAction *m_actionZoom_zoomIn = nullptr;
    QAction *m_actionZoom_zoomOut = nullptr;

    QMenu *m_menu_genePlotter = nullptr;
    QAction *m_actionShow_showGrid = nullptr;
    QAction *m_actionShow_showGenes = nullptr;
    QAction *m_actionColor_selectColorGenes = nullptr;
    QAction *m_actionColor_selectColorGrid = nullptr;

    QActionGroup *m_actionGroup_toggleVisualMode = nullptr;
    QAction *m_actionShow_toggleNormal = nullptr;
    QAction *m_actionShow_toggleDynamicRange = nullptr;
    QAction *m_actionShow_toggleDynamicRangeGenes = nullptr;
    QAction *m_actionShow_toggleHeatMap = nullptr;
 
    SpinBoxSlider *m_geneHitsThreshold = nullptr;
    QSlider *m_geneIntensitySlider = nullptr;
    QSlider *m_geneSizeSlider = nullptr;
    QSlider *m_geneShineSlider = nullptr;
    QSlider *m_geneBrightnessSlider = nullptr;

    QComboBox *m_geneShapeComboBox = nullptr;

    QMenu *m_menu_cellTissue = nullptr;
    QActionGroup *m_actionGroup_cellTissue = nullptr;
    QAction *m_actionShow_cellTissueBlue = nullptr;
    QAction *m_actionShow_cellTissueRed = nullptr;
    QAction *m_actionShow_showCellTissue = nullptr;
    QAction *m_actionRotation_rotateLeft = nullptr;
    QAction *m_actionRotation_rotateRight = nullptr;

signals:

    void thresholdLowerValueChanged(int);
    void thresholdUpperValueChanged(int);
    void intensityValueChanged(qreal);
    void sizeValueChanged(qreal);
    void shineValueChanged(qreal);
    void brightnessValueChanged(qreal);
    void shapeIndexChanged(Globals::GeneShape);
    void rotateView(qreal);

public slots:

    void resetTresholdActions(int min = 0, int max = 1);
    void resetActions();

private slots:

    void slotGeneShape(int geneShape);
    void slotGeneIntensity(int geneIntensity);
    void slotGeneSize(int geneSize);
    void slotGeneShine(int geneShine);
    void slotGeneBrightness(int geneBrightness);
    void slotRotateRight();
    void slotRotateLeft();

private:

    void createConnections();
    void createActions();
};

#endif // CELLVIEWPAGETOOLBAR_H
