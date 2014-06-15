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
    virtual ~CellViewPageToolBar();

    // actions for toolbar
    QAction *m_actionActivateSelectionMode;
    QAction *m_actionNavigate_goBack;
    QAction *m_actionNavigate_goNext;
    QAction *m_actionSave_save;
    QAction *m_actionSave_print;
    QAction *m_actionSelection_showSelectionDialog;

    QAction *m_actionZoom_zoomIn;
    QAction *m_actionZoom_zoomOut;

    QMenu *m_menu_genePlotter;
    QAction *m_actionShow_showGrid;
    QAction *m_actionShow_showGenes;
    QAction *m_actionColor_selectColorGenes;
    QAction *m_actionColor_selectColorGrid;

    QActionGroup *m_actionGroup_toggleVisualMode;
    QAction *m_actionShow_toggleNormal;
    QAction *m_actionShow_toggleDynamicRange;
    QAction *m_actionShow_toggleHeatMap;
 
    QActionGroup *m_actionGroup_toggleLegendPosition;
    QAction *m_action_toggleLegendTopRight;
    QAction *m_action_toggleLegendTopLeft;
    QAction *m_action_toggleLegendDownRight;
    QAction *m_action_toggleLegendDownLeft;

    QActionGroup *m_actionGroup_toggleMinimapPosition;
    QAction *m_action_toggleMinimapTopRight;
    QAction *m_action_toggleMinimapTopLeft;
    QAction *m_action_toggleMinimapDownRight;
    QAction *m_action_toggleMinimapDownLeft;

    SpinBoxSlider *m_geneHitsThreshold;
    QSlider *m_geneIntensitySlider;
    QSlider *m_geneSizeSlider;
    QSlider *m_geneShineSlider;
    QSlider *m_geneBrightnessSlider;

    QComboBox *m_geneShapeComboBox;

    QMenu *m_menu_cellTissue;
    QActionGroup *m_actionGroup_cellTissue;
    QAction *m_actionShow_cellTissueBlue;
    QAction *m_actionShow_cellTissueRed;
    QAction *m_actionShow_showCellTissue;
    QAction *m_actionShow_showLegend;
    QAction *m_actionShow_showMiniMap;
    QAction *m_actionRotation_rotateLeft;
    QAction *m_actionRotation_rotateRight;

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

    Q_DISABLE_COPY(CellViewPageToolBar)
};

#endif // CELLVIEWPAGETOOLBAR_H
