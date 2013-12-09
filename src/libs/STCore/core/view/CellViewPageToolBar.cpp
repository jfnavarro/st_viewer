/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "CellViewPageToolBar.h"

#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QWidgetAction>
#include <QComboBox>
#include <QToolButton>

#include "libs/libqxt/qxtspanslider.h"

CellViewPageToolBar::CellViewPageToolBar(QWidget *parent) :
    QToolBar(parent)
{
    createActions();

    setIconSize(QSize(35, 35));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    //tool bar actions
    addAction(actionNavigate_goBack);
    addSeparator();
    addAction(actionZoom_zoomIn);
    addAction(actionZoom_zoomOut);
    addSeparator();
    addAction(actionSelection_toggleSelectionMode);
    addAction(actionSelection_showSelectionDialog);
    addSeparator();
    addAction(actionSave_save);
    addAction(actionSave_print);
    addSeparator();
    // menu gene plotter actions
    menu_genePlotter = new QMenu(this);
    menu_genePlotter->setTitle(tr("Gene Plotter"));
    menu_genePlotter->addAction(actionShow_showGrid);
    menu_genePlotter->addAction(actionShow_showGenes);
    menu_genePlotter->addSeparator();
    menu_genePlotter->addAction(actionColor_selectColorGenes);
    menu_genePlotter->addAction(actionColor_selectColorGrid);
    menu_genePlotter->addSeparator();
    //color modes
    actionGroup_toggleVisualMode = new QActionGroup(menu_genePlotter);
    actionGroup_toggleVisualMode->setExclusive(true);
    actionGroup_toggleVisualMode->addAction(actionShow_toggleNormal);
    actionGroup_toggleVisualMode->addAction(actionShow_toggleDynamicRange);
    actionGroup_toggleVisualMode->addAction(actionShow_toggleHeatMap);
    menu_genePlotter->addActions(actionGroup_toggleVisualMode->actions());
    menu_genePlotter->addSeparator();
    //threshold modes
    actionGroup_toggleThresholdMode = new QActionGroup(menu_genePlotter);
    actionGroup_toggleThresholdMode->setExclusive(true);
    actionGroup_toggleThresholdMode->addAction(actionShow_toggleThresholdNormal);
    actionGroup_toggleThresholdMode->addAction(actionShow_toggleThresholdGlobal);
    menu_genePlotter->addActions(actionGroup_toggleThresholdMode->actions());
    menu_genePlotter->addSeparator();
    //threshold slider
    QxtSpanSlider *geneHitsThresholdSelector = new QxtSpanSlider();
    geneHitsThresholdSelector->setOrientation(Qt::Horizontal);
    geneHitsThresholdSelector->setHandleMovementMode(QxtSpanSlider::NoOverlapping);
    geneHitsThresholdSelector->setTickPosition(QSlider::TicksAbove);
    geneHitsThresholdSelector->setToolTip(tr("Limit of the number of transcripts."));
    actionWidget_geneHitsThreshold = new QWidgetAction(menu_genePlotter);
    actionWidget_geneHitsThreshold->setDefaultWidget(geneHitsThresholdSelector);
    menu_genePlotter->addAction(tr("Transcripts Threshold:"));
    menu_genePlotter->addAction(actionWidget_geneHitsThreshold);
    // transcripts intensity
    QSlider *geneIntesitySelector = new QSlider();
    geneIntesitySelector->setRange(Globals::gene_intensity_min,
                                   Globals::gene_intensity_max);
    geneIntesitySelector->setSingleStep(1);
    //geneIntesitySelector->setValue(Globals::gene_intensity);
    geneIntesitySelector->setInvertedAppearance(false);
    geneIntesitySelector->setInvertedControls(false);
    geneIntesitySelector->setTracking(true);
    geneIntesitySelector->setOrientation(Qt::Horizontal);
    geneIntesitySelector->setTickPosition(QSlider::TicksAbove);
    geneIntesitySelector->setTickInterval(1);
    geneIntesitySelector->setToolTip(tr("Intensity of the transcripts."));
    actionWidget_geneIntensity = new QWidgetAction(menu_genePlotter);
    actionWidget_geneIntensity->setDefaultWidget(geneIntesitySelector);
    menu_genePlotter->addAction(tr("Opacity:"));
    menu_genePlotter->addAction(actionWidget_geneIntensity);
    // transcripts size
    QSlider *geneSizeSelector = new QSlider();
    geneSizeSelector->setRange(Globals::gene_size_min,
                               Globals::gene_size_max);
    geneSizeSelector->setSingleStep(1);
    //geneSizeSelector->setValue(Globals::gene_size);
    geneSizeSelector->setOrientation(Qt::Horizontal);
    geneSizeSelector->setTickPosition(QSlider::TicksAbove);
    geneSizeSelector->setTickInterval(1);
    geneSizeSelector->setToolTip(tr("Size of the transcripts."));
    actionWidget_geneSize = new QWidgetAction(menu_genePlotter);
    actionWidget_geneSize->setDefaultWidget(geneSizeSelector);
    menu_genePlotter->addAction(tr("Size:"));
    menu_genePlotter->addAction(actionWidget_geneSize);
    menu_genePlotter->addSeparator();
    // shape of the genes
    QComboBox *geneShapeSelector = new QComboBox();
    geneShapeSelector->addItem("Circles", Globals::Circle);
    geneShapeSelector->addItem("Crosses", Globals::Cross);
    geneShapeSelector->addItem("Squares", Globals::Square);
    geneShapeSelector->setCurrentIndex(Globals::gene_shape);
    geneShapeSelector->setToolTip(tr("Shape of the genes."));
    actionWidget_geneShape = new QWidgetAction(menu_genePlotter);
    actionWidget_geneShape->setDefaultWidget(geneShapeSelector);
    menu_genePlotter->addAction(tr("Shape:"));
    menu_genePlotter->addAction(actionWidget_geneShape);
    //second menu
    QToolButton* toolButtonGene = new QToolButton();
    toolButtonGene->setMenu(menu_genePlotter);
    toolButtonGene->setPopupMode(QToolButton::InstantPopup);
    toolButtonGene->setIcon(QIcon(QStringLiteral(":/images/settings2.png")));
    toolButtonGene->setToolTip(tr("Configuration of Genes"));
    toolButtonGene->setText(tr("Configuration of Genes"));
    addWidget(toolButtonGene);
    addSeparator();
    // cell tissue menu
    menu_cellTissue = new QMenu(this);
    menu_cellTissue->setTitle(tr("Cell Tissue"));
    actionGroup_cellTissue = new QActionGroup(menu_cellTissue);
    actionGroup_cellTissue->setExclusive(true);
    actionGroup_cellTissue->addAction(actionShow_cellTissueBlue);
    actionGroup_cellTissue->addAction(actionShow_cellTissueRed);
    menu_cellTissue->addActions(actionGroup_cellTissue->actions());
    menu_cellTissue->addAction(actionShow_showCellTissue);
    QToolButton* toolButtonCell = new QToolButton();
    toolButtonCell->setMenu(menu_cellTissue);
    toolButtonCell->setPopupMode(QToolButton::InstantPopup);
    toolButtonCell->setIcon(QIcon(QStringLiteral(":/images/settings.png")));
    toolButtonCell->setToolTip(tr("Configuration of Cell Tissue"));
    toolButtonCell->setText(tr("Configuration of Cell Tissue"));
    addWidget(toolButtonCell);
    addSeparator();

    createConnections();
}

void CellViewPageToolBar::resetTresholdActions(int min, int max)
{
    QxtSpanSlider* geneHitsThreshold = qobject_cast<QxtSpanSlider*>(actionWidget_geneHitsThreshold->defaultWidget());
    //threshold slider
    geneHitsThreshold->setMinimum(min);
    geneHitsThreshold->setMaximum(max);
    geneHitsThreshold->setLowerPosition(min);
    geneHitsThreshold->setUpperPosition(max);
    geneHitsThreshold->setLowerValue(min);
    geneHitsThreshold->setUpperValue(max);
}

void CellViewPageToolBar::resetActions()
{
    // reset visual modes
    actionShow_toggleDynamicRange->setChecked(false);
    actionShow_toggleHeatMap->setChecked(false);
    actionShow_toggleNormal->setChecked(true);

    // reset threshold modes
    actionShow_toggleThresholdNormal->setChecked(true);
    actionShow_toggleThresholdGlobal->setChecked(false);

    actionShow_showCellTissue->setChecked(true);

    actionShow_showGrid->setChecked(false);

    // reset genes to show
    actionShow_showGenes->setChecked(true);

    // selection mode
    actionSelection_toggleSelectionMode->setChecked(false);

    // gene controls
    QSlider* geneIntensity = qobject_cast<QSlider*>(actionWidget_geneIntensity->defaultWidget());
    QSlider* geneSize = qobject_cast<QSlider*>(actionWidget_geneSize->defaultWidget());
    QComboBox* geneShape = qobject_cast<QComboBox*>(actionWidget_geneShape->defaultWidget());
    geneIntensity->setValue(static_cast<int>(Globals::gene_intensity) * 10); //this is ugly
    geneSize->setValue(static_cast<int>(Globals::gene_size) * 10); //this is ugly
    geneShape->setCurrentIndex(Globals::gene_shape);
}

void CellViewPageToolBar::createActions()
{
    //show grid/genes
    actionShow_showGrid = new QAction(QIcon(QStringLiteral(":/images/grid-icon-md.png")), tr("Show Grid"), this);
    actionShow_showGrid->setCheckable(true);
    actionShow_showGenes = new QAction(QIcon(QStringLiteral(":/images/genes.png")), tr("Show &Genes"), this);
    actionShow_showGenes->setCheckable(true);
    //zomming
    actionZoom_zoomIn = new QAction(QIcon(QStringLiteral(":/images/Zoom-In-icon.png")), tr("Zoom &In"), this);
    actionZoom_zoomOut = new QAction(QIcon(QStringLiteral(":/images/Zoom-Out-icon.png")), tr("Zoom &Out"), this);
    //cell tissue controls
    actionShow_cellTissueBlue = new QAction(QIcon(QStringLiteral(":/images/blue-icon.png")), tr("Load &Blue Cell Tissue"), this);
    actionShow_cellTissueBlue->setCheckable(true);
    actionShow_cellTissueRed = new QAction(QIcon(QStringLiteral(":/images/red-icon.png")), tr("Load &Red Cell Tissue"), this);
    actionShow_cellTissueRed->setCheckable(true);
    actionShow_showCellTissue = new QAction(QIcon(QStringLiteral(":/images/biology.png")), tr("Show Cell &Tissue"), this);
    actionShow_showCellTissue->setCheckable(true);
    //navigation
    actionNavigate_goBack = new QAction(QIcon(QStringLiteral(":/images/back.png")), tr("Go Back"), this);
    actionNavigate_goBack->setAutoRepeat(false);
    //color modes
    actionShow_toggleNormal = new QAction(QIcon(QStringLiteral(":/images/blue-icon.png")), tr("Normal Mode"), this);
    actionShow_toggleNormal->setCheckable(true);
    actionShow_toggleNormal->setProperty("mode", Globals::NormalMode);
    actionShow_toggleDynamicRange = new QAction(QIcon(QStringLiteral(":/images/dynamicrange.png")), tr("Dynamic Range Mode"), this);
    actionShow_toggleDynamicRange->setCheckable(true);
    actionShow_toggleDynamicRange->setProperty("mode", Globals::DynamicRangeMode);
    actionShow_toggleHeatMap = new QAction(QIcon(QStringLiteral(":/images/heatmap.png")), tr("Heat Map Mode"), this);
    actionShow_toggleHeatMap->setCheckable(true);
    actionShow_toggleHeatMap->setProperty("mode", Globals::HeatMapMode);
    //threshold modes
    actionShow_toggleThresholdNormal = new QAction(QIcon(QStringLiteral(":/images/blue-icon.png")), tr("Individual Gene Mode"), this);
    actionShow_toggleThresholdNormal->setCheckable(true);
    actionShow_toggleThresholdNormal->setProperty("mode", Globals::IndividualGeneMode);
    actionShow_toggleThresholdGlobal = new QAction(QIcon(QStringLiteral(":/images/dynamicrange.png")), tr("Global Gene Mode"), this);
    actionShow_toggleThresholdGlobal->setCheckable(true);
    actionShow_toggleThresholdGlobal->setProperty("mode", Globals::GlobalGeneMode);
    //save print
    actionSave_save = new QAction(QIcon(QStringLiteral(":/images/filesave.png")), tr("Save Cell Tissue"),  this);
    actionSave_print = new QAction(QIcon(QStringLiteral(":/images/printer.png")), tr("Print Cell Tissue"), this);
    //selection actions
    actionSelection_toggleSelectionMode = new QAction(QIcon(QStringLiteral(":/images/selection.png")), tr("Toggle Gene Selection Mode"), this);
    actionSelection_toggleSelectionMode->setCheckable(true);
    actionSelection_showSelectionDialog = new QAction(QIcon(QStringLiteral(":/images/reg_search.png")), tr("Select Genes"), this);
    // color dialogs
    actionColor_selectColorGenes = new QAction(QIcon(QStringLiteral(":/images/select-by-color-icon.png")), tr("Choose &Color Genes"), this);
    actionColor_selectColorGrid = new QAction(QIcon(QStringLiteral(":/images/edit_color.png")), tr("Choose Color Grid"), this);
}

void CellViewPageToolBar::createConnections()
{
    // gene plot signals files
    QxtSpanSlider* geneHitsThreshold = qobject_cast<QxtSpanSlider*>(actionWidget_geneHitsThreshold->defaultWidget());
    QSlider* geneIntensity = qobject_cast<QSlider*>(actionWidget_geneIntensity->defaultWidget());
    QSlider* geneSize = qobject_cast<QSlider*>(actionWidget_geneSize->defaultWidget());
    QComboBox* geneShape = qobject_cast<QComboBox*>(actionWidget_geneShape->defaultWidget());

    //threshold slider signal
    connect(geneHitsThreshold, SIGNAL(lowerValueChanged(int)), this, SIGNAL(thresholdLowerValueChanged(int)));
    connect(geneHitsThreshold, SIGNAL(upperValueChanged(int)), this, SIGNAL(thresholdUpperValueChanged(int)));
    //gene attributes signals
    connect(geneIntensity, SIGNAL(valueChanged(int)), this, SLOT(slotGeneIntensity(int)));
    connect(geneSize, SIGNAL(valueChanged(int)), this, SLOT(slotGeneSize(int)));
    connect(geneShape, SIGNAL(currentIndexChanged(int)), this, SLOT(slotGeneShape(int)));
}

void CellViewPageToolBar::slotGeneShape(int geneShape)
{
    const Globals::Shape shape = static_cast<Globals::Shape>(geneShape);
    emit shapeIndexChanged(shape);
}

//input is expected to be >= 1 and <= 10
void CellViewPageToolBar::slotGeneIntensity(int geneIntensity)
{
    const qreal decimal = static_cast<qreal>(geneIntensity) / 10;
    emit intensityValueChanged(decimal);
}

//input is expected to be >= 10 and <= 30
void CellViewPageToolBar::slotGeneSize(int geneSize)
{
    const qreal decimal = static_cast<qreal>(geneSize) / 10;
    emit sizeValueChanged(decimal);
}
