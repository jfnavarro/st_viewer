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

class CellViewPageToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit CellViewPageToolBar(QWidget *parent = 0);

    // actions for toolbar
    QAction *actionNavigate_goBack;
    QAction *actionSave_save;
    QAction *actionSave_print;
    QAction *actionSelection_toggleSelectionMode;
    QAction *actionSelection_showSelectionDialog;

    QAction *actionZoom_zoomIn;
    QAction *actionZoom_zoomOut;

    QMenu *menu_genePlotter;
    QAction *actionShow_showGrid;
    QAction *actionShow_showGenes;
    QAction *actionColor_selectColorGenes;
    QAction *actionColor_selectColorGrid;

    QActionGroup *actionGroup_toggleVisualMode;
    QAction *actionShow_toggleNormal;
    QAction *actionShow_toggleDynamicRange;
    QAction *actionShow_toggleDynamicRangeGenes;
    QAction *actionShow_toggleHeatMap;

    QActionGroup *actionGroup_toggleThresholdMode;
    QAction *actionShow_toggleThresholdNormal;
    QAction *actionShow_toggleThresholdGlobal;

    QWidgetAction *actionWidget_geneHitsThreshold;
    QWidgetAction *actionWidget_geneIntensity;
    QWidgetAction *actionWidget_geneSize;
    QWidgetAction *actionWidget_geneShape;

    QMenu *menu_cellTissue;
    QActionGroup *actionGroup_cellTissue;
    QAction *actionShow_cellTissueBlue;
    QAction *actionShow_cellTissueRed;
    QAction *actionShow_showCellTissue;

signals:
    void thresholdLowerValueChanged(int);
    void thresholdUpperValueChanged(int);
    void intensityValueChanged(qreal);
    void sizeValueChanged(qreal);
    void shapeIndexChanged(Globals::Shape);

public slots:
    void resetTresholdActions(int min = 0, int max = 1);
    void resetActions();

private slots:
    void slotGeneShape(int geneShape);
    void slotGeneIntensity(int geneIntensity);
    void slotGeneSize(int geneSize);

private:
    void createConnections();
    void createActions();
};

#endif // CELLVIEWPAGETOOLBAR_H
