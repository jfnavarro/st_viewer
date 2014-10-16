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
#include <QCoreApplication>
#include <QPushButton>
#include <QGroupBox>
#include <QRadioButton>
#include <QHBoxLayout>

#include "utils/SetTips.h"
#include "customWidgets/SpinBoxSlider.h"

static const int GENE_INTENSITY_MIN = 1;
static const int GENE_INTENSITY_MAX = 10;
static const int GENE_SIZE_MIN = 5; //10
static const int GENE_SIZE_MAX = 30;
//static const int GENE_SHIMME_MIN = 10;
//static const int GENE_SHIMME_MAX = 50;
static const int BRIGHTNESS_MIN = 1;
static const int BRIGHTNESS_MAX = 10;

namespace
{

void addWidgetToMenu(const QString &str, QMenu *menu, QWidget *widget)
{
    Q_ASSERT(menu != nullptr);
    Q_ASSERT(widget != nullptr);
    QWidgetAction *widgetAction = new QWidgetAction(menu);
    widgetAction->setDefaultWidget(widget);
    menu->addAction(str);
    menu->addAction(widgetAction);
}

void addSliderToMenu(QWidget *parent, const QString &str,
                     const QString &tooltipStr, const QString &statustipStr,
                     QMenu *menu, QSlider **slider_ptr, int rangeMin,
                     int rangeMax)
{

    Q_ASSERT(parent != nullptr);
    Q_ASSERT(slider_ptr != nullptr);
    Q_ASSERT(*slider_ptr == nullptr);
    Q_ASSERT(menu != nullptr);

    QSlider *slider = new QSlider(parent);
    *slider_ptr = slider;
    slider->setRange(rangeMin, rangeMax);
    slider->setSingleStep(1);
    slider->setValue(rangeMax);
    slider->setInvertedAppearance(false);
    slider->setInvertedControls(false);
    slider->setTracking(true);
    slider->setOrientation(Qt::Horizontal);
    slider->setTickPosition(QSlider::TicksAbove);
    slider->setTickInterval(1);
    slider->setToolTip(tooltipStr);
    slider->setStatusTip(statustipStr);
    addWidgetToMenu(str, menu, slider);
}

void createPushButton(const QString &text, const QString &tip,
                      const QIcon &icon, QWidget *parent,
                      QWidgetAction **widgetAction, QPushButton **pushButton)
{
    Q_ASSERT(*widgetAction == nullptr);
    Q_ASSERT(*pushButton == nullptr);

    QWidgetAction *action = new QWidgetAction(parent);
    QPushButton *button = new QPushButton(icon, text, parent);
    button->setMinimumSize(QSize(100,50));
    button->setMaximumSize(QSize(100,50));
    action->setDefaultWidget(button);
    button->setToolTip(tip);
    button->setStatusTip(tip);
    *widgetAction = action;
    *pushButton = button;
}

}

CellViewPageToolBar::CellViewPageToolBar(QWidget *parent) :
    QToolBar(parent),
    m_actionActivateSelectionMode(nullptr),
    m_actionNavigate_goBack(nullptr),
    m_actionNavigate_goNext(nullptr),
    m_actionSave_save(nullptr),
    m_actionSave_print(nullptr),
    m_actionSelection_showSelectionDialog(nullptr),
    m_actionZoom_zoomIn(nullptr),
    m_actionZoom_zoomOut(nullptr),
    m_actionFDH(nullptr),
    m_menu_genePlotter(nullptr),
    m_actionShow_showGrid(nullptr),
    m_actionShow_showGenes(nullptr),
    m_actionColor_selectColorGrid(nullptr),
    m_actionGroup_toggleVisualMode(nullptr),
    m_actionShow_toggleNormal(nullptr),
    m_actionShow_toggleDynamicRange(nullptr),
    m_actionShow_toggleHeatMap(nullptr),
    m_colorComputationMode(nullptr),
    m_colorLinear(nullptr),
    m_colorLog(nullptr),
    m_colorExp(nullptr),
    m_poolingMode(nullptr),
    m_poolingGenes(nullptr),
    m_poolingReads(nullptr),
    m_actionGroup_toggleLegendPosition(nullptr),
    m_action_toggleLegendTopRight(nullptr),
    m_action_toggleLegendTopLeft(nullptr),
    m_action_toggleLegendDownRight(nullptr),
    m_action_toggleLegendDownLeft(nullptr),
    m_actionGroup_toggleMinimapPosition(nullptr),
    m_action_toggleMinimapTopRight(nullptr),
    m_action_toggleMinimapTopLeft(nullptr),
    m_action_toggleMinimapDownRight(nullptr),
    m_action_toggleMinimapDownLeft(nullptr),
    m_geneHitsThreshold(nullptr),
    m_geneIntensitySlider(nullptr),
    m_geneSizeSlider(nullptr),
    m_geneShineSlider(nullptr),
    m_geneBrightnessSlider(nullptr),
    m_geneShapeComboBox(nullptr),
    m_menu_cellTissue(nullptr),
    m_actionGroup_cellTissue(nullptr),
    m_actionShow_cellTissueBlue(nullptr),
    m_actionShow_cellTissueRed(nullptr),
    m_actionShow_showCellTissue(nullptr),
    m_actionShow_showLegend(nullptr),
    m_actionShow_showMiniMap(nullptr),
    m_actionRotation_rotateLeft(nullptr),
    m_actionRotation_rotateRight(nullptr),
    m_buttonNavigate_goBack(nullptr),
    m_buttonNavigate_goNext(nullptr)
{
    createActions();

    setIconSize(QSize(35, 35));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    //tool bar actions
    addAction(m_actionNavigate_goBack);
    addSeparator();
    addAction(m_actionZoom_zoomIn);
    addAction(m_actionZoom_zoomOut);
    addSeparator();
    addAction(m_actionFDH);
    addSeparator();
    addAction(m_actionActivateSelectionMode);
    addAction(m_actionSelection_showSelectionDialog);
    addSeparator();
    addAction(m_actionSave_save);
    addAction(m_actionSave_print);
    addSeparator();

    // menu gene plotter actions
    m_menu_genePlotter = new QMenu(this);
    m_menu_genePlotter->setTitle(tr("Gene Plotter"));
    setToolTipAndStatusTip(tr("Tools for visualization of genes"), m_menu_genePlotter);
    m_menu_genePlotter->addAction(m_actionShow_showGrid);
    m_menu_genePlotter->addAction(m_actionShow_showGenes);
    m_menu_genePlotter->addSeparator();
    m_menu_genePlotter->addAction(m_actionColor_selectColorGrid);
    m_menu_genePlotter->addSeparator();

    //color modes
    m_actionGroup_toggleVisualMode = new QActionGroup(m_menu_genePlotter);
    m_actionGroup_toggleVisualMode->setExclusive(true);
    m_actionGroup_toggleVisualMode->addAction(m_actionShow_toggleNormal);
    m_actionGroup_toggleVisualMode->addAction(m_actionShow_toggleDynamicRange);
    m_actionGroup_toggleVisualMode->addAction(m_actionShow_toggleHeatMap);
    m_menu_genePlotter->addActions(m_actionGroup_toggleVisualMode->actions());
    m_menu_genePlotter->addSeparator();

    //color
    m_colorComputationMode = new QGroupBox(this);
    m_colorComputationMode->setFlat(true);
    setToolTipAndStatusTip(
            tr("Choses the type of color computation."),
            m_colorComputationMode);
    m_colorLinear = new QRadioButton(tr("Linear"));
    m_colorLog = new QRadioButton(tr("Log"));
    m_colorExp = new QRadioButton(tr("Exp"));
    m_colorLinear->setChecked(true);
    QHBoxLayout *hboxColor = new QHBoxLayout();
    hboxColor->addWidget(m_colorLinear);
    hboxColor->addWidget(m_colorLog);
    hboxColor->addWidget(m_colorExp);
    hboxColor->addStretch(1);
    m_colorComputationMode->setLayout(hboxColor);
    addWidgetToMenu(tr("Color computation:"), m_menu_genePlotter, m_colorComputationMode);

    //color
    m_poolingMode = new QGroupBox(this);
    m_poolingMode->setFlat(true);
    setToolTipAndStatusTip(
            tr("Choses the type of pooling mode for dynamic range and heatmap."),
            m_poolingMode);
    m_poolingGenes = new QRadioButton(tr("Genes"));
    m_poolingReads = new QRadioButton(tr("Reads"));
    m_poolingReads->setChecked(true);
    QHBoxLayout *hboxPooling = new QHBoxLayout();
    hboxPooling->addWidget(m_poolingReads);
    hboxPooling->addWidget(m_poolingGenes);
    hboxPooling->addStretch(1);
    m_poolingMode->setLayout(hboxPooling);
    addWidgetToMenu(tr("Pooling modes:"), m_menu_genePlotter, m_poolingMode);

    m_menu_genePlotter->addSeparator();

    //threshold slider
    Q_ASSERT(m_geneHitsThreshold == nullptr);
    m_geneHitsThreshold = new SpinBoxSlider(this, SpinBoxSlider::onlySpinBoxes);
    setToolTipAndStatusTip(
            tr("Limit of the number of reads per feature."),
            m_geneHitsThreshold);
    addWidgetToMenu(tr("Transcripts Threshold:"), m_menu_genePlotter, m_geneHitsThreshold);

    // transcripts intensity
    addSliderToMenu(this,
                    tr("Opacity:"),
                    tr("Set the intensity of the genes"),
                    tr("Set the intensity of the genes"),
                    m_menu_genePlotter,
                    &m_geneIntensitySlider,
                    GENE_INTENSITY_MIN,
                    GENE_INTENSITY_MAX);
    addSliderToMenu(this,
                    tr("Size:"),
                    tr("Set the size of the genes"),
                    tr("Set the size of the genes"),
                    m_menu_genePlotter,
                    &m_geneSizeSlider,
                    GENE_SIZE_MIN,
                    GENE_SIZE_MAX);
    // addSliderToMenu(this,
    //                 tr("Shine:"),
    //                 tr("Shine level of the genes"),
    //                 m_menu_genePlotter,
    //                 &m_geneShineSlider,
    //                 GENE_SHIMME_MIN,
    //                 GENE_SHIMME_MAX);

    m_menu_genePlotter->addSeparator();

    // shape of the genes
    Q_ASSERT(m_geneShapeComboBox == nullptr);
    m_geneShapeComboBox = new QComboBox(this);
    m_geneShapeComboBox->addItem("Circles", Globals::GeneShape::Circle);
    m_geneShapeComboBox->addItem("Crosses", Globals::GeneShape::Cross);
    m_geneShapeComboBox->addItem("Squares", Globals::GeneShape::Square);
    m_geneShapeComboBox->setCurrentIndex(Globals::GeneShape::Circle);
    m_geneShapeComboBox->setToolTip(tr("Set the shape of the genes"));
    m_geneShapeComboBox->setStatusTip(tr("Set the shape of the genes"));
    addWidgetToMenu(tr("Shape:"), m_menu_genePlotter, m_geneShapeComboBox);

    //second menu
    QToolButton* toolButtonGene = new QToolButton(this);
    toolButtonGene->setMenu(m_menu_genePlotter);
    toolButtonGene->setPopupMode(QToolButton::InstantPopup);
    toolButtonGene->setIcon(QIcon(QStringLiteral(":/images/settings2.png")));
    toolButtonGene->setToolTip(tr("Configuration of Genes"));
    toolButtonGene->setStatusTip(tr("Configuration of Genes"));
    toolButtonGene->setText(tr("Configuration of Genes"));
    addWidget(toolButtonGene);
    addSeparator();

    // cell tissue menu
    m_menu_cellTissue = new QMenu(this);
    m_menu_cellTissue->setTitle(tr("Cell Tissue"));
    setToolTipAndStatusTip(tr("Tools for visualization of the cell tissue"), m_menu_cellTissue);
    m_menu_cellTissue->addAction(m_actionShow_showMiniMap);
    m_menu_cellTissue->addAction(m_actionShow_showLegend);

    m_menu_cellTissue->addSeparator()->setText(tr("Legend Position"));
    m_actionGroup_toggleLegendPosition = new QActionGroup(m_menu_cellTissue);
    m_actionGroup_toggleLegendPosition->setExclusive(true);
    m_actionGroup_toggleLegendPosition->addAction(m_action_toggleLegendTopRight);
    m_actionGroup_toggleLegendPosition->addAction(m_action_toggleLegendTopLeft);
    m_actionGroup_toggleLegendPosition->addAction(m_action_toggleLegendDownRight);
    m_actionGroup_toggleLegendPosition->addAction(m_action_toggleLegendDownLeft);
    m_menu_cellTissue->addActions(m_actionGroup_toggleLegendPosition->actions());
    m_menu_cellTissue->addSeparator();

    m_menu_cellTissue->addSeparator()->setText(tr("Minimap Position"));
    m_actionGroup_toggleMinimapPosition = new QActionGroup(m_menu_cellTissue);
    m_actionGroup_toggleMinimapPosition->setExclusive(true);
    m_actionGroup_toggleMinimapPosition->addAction(m_action_toggleMinimapTopRight);
    m_actionGroup_toggleMinimapPosition->addAction(m_action_toggleMinimapTopLeft);
    m_actionGroup_toggleMinimapPosition->addAction(m_action_toggleMinimapDownRight);
    m_actionGroup_toggleMinimapPosition->addAction(m_action_toggleMinimapDownLeft);
    m_menu_cellTissue->addActions(m_actionGroup_toggleMinimapPosition->actions());
    m_menu_cellTissue->addSeparator();

    //We don't show the rotate actions in the GUI before we have fixed the rotation functionality
    //m_menu_cellTissue->addAction(m_actionRotation_rotateLeft);
    //m_menu_cellTissue->addAction(m_actionRotation_rotateRight);
    m_menu_cellTissue->addSeparator();

    m_actionGroup_cellTissue = new QActionGroup(m_menu_cellTissue);
    m_actionGroup_cellTissue->setExclusive(true);
    m_actionGroup_cellTissue->addAction(m_actionShow_cellTissueBlue);
    m_actionGroup_cellTissue->addAction(m_actionShow_cellTissueRed);
    m_menu_cellTissue->addActions(m_actionGroup_cellTissue->actions());
    m_menu_cellTissue->addAction(m_actionShow_showCellTissue);

    m_menu_cellTissue->addSeparator();

    addSliderToMenu(this,
                    tr("Brightness:"),
                    tr("Brightness level of the Cell Tissue"),
                    tr("Brightness level of the Cell Tissue"),
                    m_menu_cellTissue,
                    &m_geneBrightnessSlider,
                    BRIGHTNESS_MIN,
                    BRIGHTNESS_MAX);

    QToolButton *toolButtonCell = new QToolButton();
    toolButtonCell->setMenu(m_menu_cellTissue);
    toolButtonCell->setPopupMode(QToolButton::InstantPopup);
    toolButtonCell->setIcon(QIcon(QStringLiteral(":/images/settings.png")));
    setToolTipAndStatusTip(tr("Configuration of Cell Tissue"), toolButtonCell);
    toolButtonCell->setText(tr("Configuration of Cell Tissue"));
    addWidget(toolButtonCell);
    addSeparator();

    //next button
    addAction(m_actionNavigate_goNext);

    createConnections();
}

CellViewPageToolBar::~CellViewPageToolBar()
{

}

void CellViewPageToolBar::setEnableButtons(bool enable)
{
    m_actionActivateSelectionMode->setEnabled(enable);
    m_actionSave_save->setEnabled(enable);
    m_actionSave_print->setEnabled(enable);
    m_actionSelection_showSelectionDialog->setEnabled(enable);
    m_actionZoom_zoomIn->setEnabled(enable);
    m_actionZoom_zoomOut->setEnabled(enable);
    m_actionFDH->setEnabled(enable);
    m_menu_genePlotter->setEnabled(enable);
    m_menu_cellTissue->setEnabled(enable);
}

void CellViewPageToolBar::resetTresholdActions(int min, int max)
{
    Q_ASSERT(m_geneHitsThreshold != nullptr);

    //threshold slider
    m_geneHitsThreshold->setMinimumValue(min);
    m_geneHitsThreshold->setMaximumValue(max);
    m_geneHitsThreshold->setLowerValue(min);
    m_geneHitsThreshold->setUpperValue(max);
    m_geneHitsThreshold->setTickInterval(1);
}

void CellViewPageToolBar::resetActions()
{
    // reset visual modes
    m_actionShow_toggleDynamicRange->setChecked(false);
    m_actionShow_toggleHeatMap->setChecked(false);
    m_actionShow_toggleNormal->setChecked(true);

    // reset show cell tissue
    m_actionShow_showCellTissue->setChecked(true);

    // reset show grids
    m_actionShow_showGrid->setChecked(false);

    // reset genes to show
    m_actionShow_showGenes->setChecked(true);

    // gene controls
    m_geneIntensitySlider->setValue(GENE_INTENSITY_MAX);
    m_geneSizeSlider->setValue(GENE_SIZE_MIN);
    m_geneShapeComboBox->setCurrentIndex(Globals::GeneShape::Circle);

    // selection mode
    m_actionActivateSelectionMode->setChecked(false);

    // anchor signals
    m_action_toggleLegendTopRight->setChecked(true);
    m_action_toggleLegendTopLeft->setChecked(false);
    m_action_toggleLegendDownRight->setChecked(false);
    m_action_toggleLegendDownLeft->setChecked(false);

    m_action_toggleMinimapTopRight->setChecked(false);
    m_action_toggleMinimapTopLeft->setChecked(false);
    m_action_toggleMinimapDownRight->setChecked(true);
    m_action_toggleMinimapDownLeft->setChecked(false);

    // show legend and minimap
    m_actionShow_showLegend->setChecked(false);
    m_actionShow_showMiniMap->setChecked(true);
}

void CellViewPageToolBar::createActions()
{
    //show grid/genes
    m_actionShow_showGrid =
            new QAction(QIcon(QStringLiteral(":/images/grid-icon-md.png")), tr("Show Grid"), this);
    m_actionShow_showGrid->setCheckable(true);
    m_actionShow_showGenes =
            new QAction(QIcon(QStringLiteral(":/images/genes.png")), tr("Show Genes"), this);
    m_actionShow_showGenes->setCheckable(true);

    //zomming
    m_actionZoom_zoomIn =
            new QAction(QIcon(QStringLiteral(":/images/Zoom-In-icon.png")), tr("Zoom &In"), this);
    setToolTipAndStatusTip(tr("Increases the zoom level in the cell tissue"), m_actionZoom_zoomIn);
    m_actionZoom_zoomOut =
            new QAction(QIcon(QStringLiteral(":/images/Zoom-Out-icon.png")), tr("Zoom &Out"), this);
    setToolTipAndStatusTip(tr("Decreases the zoom level in the cell tissue"), m_actionZoom_zoomOut);

    // FDH Widget
    m_actionFDH =
            new QAction(QIcon(QStringLiteral(":/images/histogram.png")), tr("FDH"), this);
    setToolTipAndStatusTip(tr("Shows/hides the Features Distribution Histogram"), m_actionFDH);

    //cell tissue controls
    m_actionShow_cellTissueBlue =
            new QAction(QIcon(QStringLiteral(":/images/blue-icon.png")),
                        tr("Load Blue Cell Tissue"), this);
    m_actionShow_cellTissueBlue->setCheckable(true);
    m_actionShow_cellTissueRed =
            new QAction(QIcon(QStringLiteral(":/images/red-icon.png")),
                        tr("Load Red Cell Tissue"), this);
    m_actionShow_cellTissueRed->setCheckable(true);
    m_actionShow_showCellTissue =
            new QAction(QIcon(QStringLiteral(":/images/biology.png")),
                        tr("Show Cell Tissue"), this);
    m_actionShow_showCellTissue->setCheckable(true);

    // navigation push buttons
    createPushButton(tr("Back"), tr("Go back to Dataset Page"),
                     QIcon(QStringLiteral(":/images/back.png")),
                     this, &m_actionNavigate_goBack, &m_buttonNavigate_goBack);
    createPushButton(tr("Next"), tr("Go to Analysis Page"),
                     QIcon(QStringLiteral(":/images/next.png")),
                     this, &m_actionNavigate_goNext, &m_buttonNavigate_goNext);

    //color modes
    m_actionShow_toggleNormal =
            new QAction(QIcon(QStringLiteral(":/images/blue-icon.png")), tr("Normal Mode"), this);
    setToolTipAndStatusTip(
            tr("Color mode where the genes are treated individually per feature."),
            m_actionShow_toggleNormal);
    m_actionShow_toggleNormal->setCheckable(true);
    m_actionShow_toggleNormal->setProperty("mode", Globals::GeneVisualMode::NormalMode);
    m_actionShow_toggleDynamicRange =
            new QAction(QIcon(QStringLiteral(":/images/dynamicrange.png")),
                        tr("Dynamic Range Mode"), this);
    setToolTipAndStatusTip(
            tr("Color mode where the features tranparency is related to the level of expression"),
            m_actionShow_toggleDynamicRange);
    m_actionShow_toggleDynamicRange->setCheckable(true);
    m_actionShow_toggleDynamicRange->setProperty("mode", Globals::GeneVisualMode::DynamicRangeMode);
    m_actionShow_toggleHeatMap =
            new QAction(QIcon(QStringLiteral(":/images/heatmap.png")), tr("Heat Map Mode"), this);
    setToolTipAndStatusTip(
            tr("Color mode where the color is computed according to the level of expression"),
            m_actionShow_toggleHeatMap);
    m_actionShow_toggleHeatMap->setCheckable(true);
    m_actionShow_toggleHeatMap->setProperty("mode", Globals::GeneVisualMode::HeatMapMode);

    // legend position
    m_action_toggleLegendTopRight = new QAction(QIcon(), tr("Legend Top Right Corner"), this);
    setToolTipAndStatusTip(
            tr("Places the legend on the top right part of the canvas"),
            m_action_toggleLegendTopRight);
    m_action_toggleLegendTopRight->setCheckable(true);
    m_action_toggleLegendTopRight->setProperty("mode", Globals::Anchor::NorthEast);
    m_action_toggleLegendTopLeft = new QAction(QIcon(), tr("Legend Top Left Corner"), this);
    setToolTipAndStatusTip(
            tr("Places the legend on the top left part of the canvas"),
            m_action_toggleLegendTopLeft);
    m_action_toggleLegendTopLeft->setCheckable(true);
    m_action_toggleLegendTopLeft->setProperty("mode", Globals::Anchor::NorthWest);
    m_action_toggleLegendDownRight = new QAction(QIcon(), tr("Legend Bottom Right Corner"), this);
    setToolTipAndStatusTip(
            tr("Places the legend on the bottom right part of the canvas"),
            m_action_toggleLegendDownRight);
    m_action_toggleLegendDownRight->setCheckable(true);
    m_action_toggleLegendDownRight->setProperty("mode", Globals::Anchor::SouthEast);
    m_action_toggleLegendDownLeft= new QAction(QIcon(), tr("Legend Bottom Right Corner"), this);
    setToolTipAndStatusTip(
            tr("Places the legend on the bottom left part of the canvas"),
            m_action_toggleLegendDownLeft);
    m_action_toggleLegendDownLeft->setCheckable(true);
    m_action_toggleLegendDownLeft->setProperty("mode", Globals::Anchor::SouthWest);

    // show legend
    m_actionShow_showLegend = new QAction(QIcon(), tr("Show Legend"), this);
    m_actionShow_showLegend->setCheckable(true);

    // minimap position
    m_action_toggleMinimapTopRight = new QAction(QIcon(), tr("Minimap Top Right Corner"), this);
    setToolTipAndStatusTip(
            tr("Places the minimap on the top right part of the canvas"),
            m_action_toggleMinimapTopRight);
    m_action_toggleMinimapTopRight->setCheckable(true);
    m_action_toggleMinimapTopRight->setProperty("mode", Globals::Anchor::NorthEast);
    m_action_toggleMinimapTopLeft = new QAction(QIcon(), tr("Minimap Top Left Corner"), this);
    setToolTipAndStatusTip(
            tr("Places the minimap on the top left part of the canvas"),
            m_action_toggleMinimapTopLeft);
    m_action_toggleMinimapTopLeft->setCheckable(true);
    m_action_toggleMinimapTopLeft->setProperty("mode", Globals::Anchor::NorthWest);
    m_action_toggleMinimapDownRight = new QAction(QIcon(), tr("Bottom Right Corner"), this);
    setToolTipAndStatusTip(
            tr("Places the minimap on the bottom right part of the canvas"),
            m_action_toggleMinimapDownRight);
    m_action_toggleMinimapDownRight->setCheckable(true);
    m_action_toggleMinimapDownRight->setProperty("mode", Globals::Anchor::SouthEast);
    m_action_toggleMinimapDownLeft = new QAction(QIcon(), tr("Legend Bottom Right Corner"), this);
    setToolTipAndStatusTip(
            tr("Places the minimap on the bottom left part of the canvas"),
            m_action_toggleMinimapDownLeft);
    m_action_toggleMinimapDownLeft->setCheckable(true);
    m_action_toggleMinimapDownLeft->setProperty("mode", Globals::Anchor::SouthWest);

    // show minimap
    m_actionShow_showMiniMap = new QAction(QIcon(), tr("Show MiniMap"), this);
    m_actionShow_showMiniMap->setCheckable(true);

    //save print
    m_actionSave_save =
            new QAction(QIcon(QStringLiteral(":/images/filesave.png")),
                        tr("Save Cell Tissue"),  this);

    setToolTipAndStatusTip(
            tr("Save the cell tissue canvas into a file"),
            m_actionSave_save);
    m_actionSave_print =
            new QAction(QIcon(QStringLiteral(":/images/printer.png")),
                        tr("Print Cell Tissue"), this);
    setToolTipAndStatusTip(
            tr("Print the cell tissue canvas"),
            m_actionSave_print);

    //selection actions
    m_actionActivateSelectionMode =
            new QAction(QIcon(QStringLiteral(":/images/selection.png")),
                        tr("Activate Selection Mode"), this);
    m_actionActivateSelectionMode->setCheckable(true);
    setToolTipAndStatusTip(
            tr("Activates/desactivates the selection mode"),
            m_actionActivateSelectionMode);
    m_actionSelection_showSelectionDialog =
            new QAction(QIcon(QStringLiteral(":/images/reg_search.png")), tr("Select Genes"), this);
    setToolTipAndStatusTip(
            tr("Shows a dialog to select genes by Reg. Exp."),
            m_actionSelection_showSelectionDialog);

    // color dialogs
    m_actionColor_selectColorGrid =
            new QAction(QIcon(QStringLiteral(":/images/edit_color.png")),
                        tr("Choose Color Grid"), this);

    m_actionRotation_rotateLeft =
            new QAction(QIcon(QStringLiteral(":/images/rotate_left.png")),
                        tr("Rotate &left"), this);
    m_actionRotation_rotateRight =
            new QAction(QIcon(QStringLiteral(":/images/rotate_right.png")),
                        tr("Rotate &right"), this);
}

void CellViewPageToolBar::createConnections()
{
    // navigation signals
    connect(m_buttonNavigate_goBack, SIGNAL(clicked(bool)), m_actionNavigate_goBack, SLOT(trigger()));
    connect(m_buttonNavigate_goNext, SIGNAL(clicked(bool)), m_actionNavigate_goNext, SLOT(trigger()));

    //threshold slider signal
    connect(m_geneHitsThreshold, SIGNAL(lowerValueChanged(int)),
            this, SIGNAL(thresholdLowerValueChanged(int)));
    connect(m_geneHitsThreshold, SIGNAL(upperValueChanged(int)),
            this, SIGNAL(thresholdUpperValueChanged(int)));

    //gene attributes signals
    connect(m_geneIntensitySlider, SIGNAL(valueChanged(int)), this, SLOT(slotGeneIntensity(int)));
    connect(m_geneSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(slotGeneSize(int)));
    //    Q_ASSERT(m_geneShineSlider);
    //    connect(m_geneShineSlider, SIGNAL(valueChanged(int)), this, SLOT(slotGeneShine(int)));
    connect(m_geneBrightnessSlider, SIGNAL(valueChanged(int)), this, SLOT(slotGeneBrightness(int)));
    connect(m_geneShapeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotGeneShape(int)));
    connect(m_actionRotation_rotateLeft, SIGNAL(triggered(bool)), this, SLOT(slotRotateLeft()));
    connect(m_actionRotation_rotateRight, SIGNAL(triggered(bool)), this, SLOT(slotRotateRight()));

    //color and pooling signals
    connect(m_colorExp, SIGNAL(clicked()), this, SLOT(slotColorExp()));
    connect(m_colorLinear, SIGNAL(clicked()), this, SLOT(slotColorLinear()));
    connect(m_colorLog, SIGNAL(clicked()), this, SLOT(slotColorLog()));
    connect(m_poolingGenes, SIGNAL(clicked()), this, SLOT(slotPoolingGenes()));
    connect(m_poolingReads, SIGNAL(clicked()), this, SLOT(slotPoolingReads()));
}

void CellViewPageToolBar::slotGeneShape(int geneShape)
{
    const Globals::GeneShape shape = static_cast<Globals::GeneShape>(geneShape);
    emit shapeIndexChanged(shape);
}

//input is expected to be >= 1 and <= 10
void CellViewPageToolBar::slotGeneIntensity(int geneIntensity)
{
    Q_ASSERT(geneIntensity >= 1 && geneIntensity <= 10);
    const qreal decimal = static_cast<qreal>(geneIntensity) / 10;
    emit intensityValueChanged(decimal);
}

//input is expected to be >= 5 and <= 30
void CellViewPageToolBar::slotGeneSize(int geneSize)
{
    Q_ASSERT(geneSize >= 5 && geneSize <= 30);
    const qreal decimal = static_cast<qreal>(geneSize) / 10;
    emit sizeValueChanged(decimal);
}

//input is expected to be >= 1 and <= 10
void CellViewPageToolBar::slotGeneShine(int geneShine)
{
    Q_ASSERT(geneShine >= 1 && geneShine <= 10);
    const qreal decimal = static_cast<qreal>(geneShine) / 10;
    emit shineValueChanged(decimal);
}

//input is expected to be >= 1 and <= 10
void CellViewPageToolBar::slotGeneBrightness(int geneBrightness)
{
    Q_ASSERT(geneBrightness >= 1 && geneBrightness <= 10);
    const qreal decimal = static_cast<qreal>(geneBrightness) / 10;
    emit brightnessValueChanged(decimal);
}

void CellViewPageToolBar::slotRotateLeft()
{
    emit rotateView(-45.0f);
}

void CellViewPageToolBar::slotRotateRight()
{
    emit rotateView(45.0f);
}

void CellViewPageToolBar::slotColorExp()
{
    emit colorComputationChanged(Globals::ExpColor);
}

void CellViewPageToolBar::slotColorLog()
{
    emit colorComputationChanged(Globals::LogColor);
}

void CellViewPageToolBar::slotColorLinear()
{
    emit colorComputationChanged(Globals::LinearColor);
}

void CellViewPageToolBar::slotPoolingGenes()
{
    emit poolingModeChanged(Globals::PoolNumberGenes);
}

void CellViewPageToolBar::slotPoolingReads()
{
    emit poolingModeChanged(Globals::PoolReadsCount);
}
