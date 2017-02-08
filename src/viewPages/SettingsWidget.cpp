#include "SettingsWidget.h"
#include "ui_SettingsWidget.h"

#include <QWidgetAction>
#include <QComboBox>
#include <QSlider>
#include <QRadioButton>
#include <QGroupBox>
#include <QMenu>
#include <QColorDialog>
#include "customWidgets/SpinBoxSlider.h"

static const int GENE_INTENSITY_MIN = 1;
static const int GENE_INTENSITY_MAX = 10;
static const int GENE_SIZE_MIN = 5;
static const int GENE_SIZE_MAX = 30;

SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);

    // set some default properties for some actions
    m_ui->actionShow_toggleNormal->setProperty("mode", GeneRendererGL::NormalMode);
    m_ui->actionShow_toggleDynamicRange->setProperty("mode", GeneRendererGL::DynamicRangeMode);
    m_ui->actionShow_toggleHeatMap->setProperty("mode", GeneRendererGL::HeatMapMode);
    m_ui->action_toggleLegendTopRight->setProperty("mode", Anchor::NorthEast);
    m_ui->action_toggleLegendTopLeft->setProperty("mode", Anchor::NorthWest);
    m_ui->action_toggleLegendDownRight->setProperty("mode", Anchor::SouthEast);
    m_ui->action_toggleLegendDownLeft->setProperty("mode", Anchor::SouthWest);

    // menu gene plotter actions
    QMenu *menu_genePlotter = new QMenu(this);
    menu_genePlotter->setTitle(tr("Gene Plotter"));
    setToolTipAndStatusTip(tr("Tools for visualization of genes"), menu_genePlotter);
    menu_genePlotter->addAction(m_ui->actionShow_showGrid);
    menu_genePlotter->addAction(m_ui->actionShow_showGenes);
    menu_genePlotter->addSeparator();
    menu_genePlotter->addAction(m_ui->actionColor_selectColorGrid);
    menu_genePlotter->addSeparator();

    // color modes
    QActionGroup *actionGroup_toggleVisualMode = new QActionGroup(menu_genePlotter);
    actionGroup_toggleVisualMode->setExclusive(true);
    actionGroup_toggleVisualMode->addAction(m_ui->actionShow_toggleNormal);
    actionGroup_toggleVisualMode->addAction(m_ui->actionShow_toggleDynamicRange);
    actionGroup_toggleVisualMode->addAction(m_ui->actionShow_toggleHeatMap);
    menu_genePlotter->addActions(actionGroup_toggleVisualMode->actions());
    menu_genePlotter->addSeparator();

    // color computation modes
    QGroupBox *colorComputationMode = new QGroupBox(this);
    colorComputationMode->setFlat(true);
    setToolTipAndStatusTip(tr("Chooses the type of color computation."), colorComputationMode);
    m_colorLinear.reset(new QRadioButton(tr("Linear")));
    m_colorLog.reset(new QRadioButton(tr("Log")));
    m_colorExp.reset(new QRadioButton(tr("Exp")));
    m_colorLinear->setChecked(true);
    QHBoxLayout *hboxColor = new QHBoxLayout();
    hboxColor->addWidget(m_colorLinear.data());
    hboxColor->addWidget(m_colorLog.data());
    hboxColor->addWidget(m_colorExp.data());
    hboxColor->addStretch(1);
    colorComputationMode->setLayout(hboxColor);
    addWidgetToMenu(tr("Color computation:"), menu_genePlotter, colorComputationMode);

    // color modes
    QGroupBox *poolingMode = new QGroupBox(this);
    poolingMode->setFlat(true);
    setToolTipAndStatusTip(tr("Chooses the type of pooling mode for dynamic range and heatmap."),
                           poolingMode);
    m_poolingGenes.reset(new QRadioButton(tr("Genes")));
    m_poolingReads.reset(new QRadioButton(tr("Reads")));
    m_poolingTPMs.reset(new QRadioButton(tr("TPM")));
    m_poolingReads->setChecked(true);
    QHBoxLayout *hboxPooling = new QHBoxLayout();
    hboxPooling->addWidget(m_poolingReads.data());
    hboxPooling->addWidget(m_poolingGenes.data());
    hboxPooling->addWidget(m_poolingTPMs.data());
    hboxPooling->addStretch(1);
    poolingMode->setLayout(hboxPooling);
    addWidgetToMenu(tr("Pooling modes:"), menu_genePlotter, poolingMode);
    menu_genePlotter->addSeparator();

    // threshold reads slider
    m_geneHitsThreshold.reset(new SpinBoxSlider(this, SpinBoxSlider::onlySpinBoxes));
    setToolTipAndStatusTip(tr("Limit of the number of reads per genes."),
                           m_geneHitsThreshold.data());
    addWidgetToMenu(tr("Reads Threshold:"), menu_genePlotter, m_geneHitsThreshold.data());
    menu_genePlotter->addSeparator();

    // threshold genes slider
    m_geneGenesThreshold.reset(new SpinBoxSlider(this, SpinBoxSlider::onlySpinBoxes));
    setToolTipAndStatusTip(tr("Limit of the number of genes per feature."),
                           m_geneHitsThreshold.data());
    addWidgetToMenu(tr("Genes Threshold:"), menu_genePlotter, m_geneGenesThreshold.data());
    menu_genePlotter->addSeparator();

    // threshold total reads slider
    m_geneTotalReadsThreshold.reset(new SpinBoxSlider(this, SpinBoxSlider::onlySpinBoxes));
    setToolTipAndStatusTip(tr("Limit of the number of total reads per feature."),
                           m_geneHitsThreshold.data());
    addWidgetToMenu(tr("Total Reads Threshold:"),
                    menu_genePlotter,
                    m_geneTotalReadsThreshold.data());
    menu_genePlotter->addSeparator();

    // Individual gene cut-off
    menu_genePlotter->addAction(m_ui->actionIndividual_gene_cut_off);
    menu_genePlotter->addSeparator();

    // transcripts intensity and size sliders
    m_geneIntensitySlider.reset(new QSlider(this));
    addSliderToMenu(this,
                    tr("Opacity:"),
                    tr("Set the intensity of the genes"),
                    tr("Set the intensity of the genes"),
                    menu_genePlotter,
                    m_geneIntensitySlider.data(),
                    GENE_INTENSITY_MIN,
                    GENE_INTENSITY_MAX);
    m_geneSizeSlider.reset(new QSlider(this));
    addSliderToMenu(this,
                    tr("Size:"),
                    tr("Set the size of the genes"),
                    tr("Set the size of the genes"),
                    menu_genePlotter,
                    m_geneSizeSlider.data(),
                    GENE_SIZE_MIN,
                    GENE_SIZE_MAX);
    menu_genePlotter->addSeparator();

    // shape of the genes
    m_geneShapeComboBox.reset(new QComboBox(this));
    m_geneShapeComboBox->addItem("Circles", GeneRendererGL::Circle);
    m_geneShapeComboBox->addItem("Crosses", GeneRendererGL::Cross);
    m_geneShapeComboBox->addItem("Squares", GeneRendererGL::Square);
    m_geneShapeComboBox->setCurrentIndex(GeneRendererGL::Circle);
    setToolTipAndStatusTip(tr("Set the shape of the genes"), m_geneShapeComboBox.data());
    addWidgetToMenu(tr("Shape:"), menu_genePlotter, m_geneShapeComboBox.data());

    // add menu to toolbutton in top bar
    m_ui->genemenu->setMenu(menu_genePlotter);

    // cell tissue menu
    QMenu *menu_cellTissue = new QMenu(this);
    menu_cellTissue->setTitle(tr("Cell Tissue"));
    setToolTipAndStatusTip(tr("Tools for visualization of the cell tissue"), menu_cellTissue);
    menu_cellTissue->addAction(m_ui->actionShow_showLegend);

    // group legend positions actions into one
    menu_cellTissue->addSeparator()->setText(tr("Legend Position"));
    QActionGroup *actionGroup_toggleLegendPosition = new QActionGroup(menu_cellTissue);
    actionGroup_toggleLegendPosition->setExclusive(true);
    actionGroup_toggleLegendPosition->addAction(m_ui->action_toggleLegendTopRight);
    actionGroup_toggleLegendPosition->addAction(m_ui->action_toggleLegendTopLeft);
    actionGroup_toggleLegendPosition->addAction(m_ui->action_toggleLegendDownRight);
    actionGroup_toggleLegendPosition->addAction(m_ui->action_toggleLegendDownLeft);
    menu_cellTissue->addActions(actionGroup_toggleLegendPosition->actions());
    menu_cellTissue->addSeparator();

    // load red/blue actions and show/hide action for cell tissue
    QActionGroup *actionGroup_cellTissue = new QActionGroup(menu_cellTissue);
    actionGroup_cellTissue->setExclusive(true);
    actionGroup_cellTissue->addAction(m_ui->actionShow_cellTissueBlue);
    actionGroup_cellTissue->addAction(m_ui->actionShow_cellTissueRed);
    menu_cellTissue->addActions(actionGroup_cellTissue->actions());
    menu_cellTissue->addAction(m_ui->actionShow_showCellTissue);
    menu_cellTissue->addSeparator();

    // add menu to tool button in top bar
    m_ui->cellmenu->setMenu(menu_cellTissue);
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
}


void CellViewPage::resetActionStates()
{
    // reset color dialogs
    m_colorDialogGrid->setCurrentColor(GridRendererGL::DEFAULT_COLOR_GRID);

    // reset cell image to show
    m_image->setVisible(true);
    m_image->setAnchor(DEFAULT_ANCHOR_IMAGE);

    // reset gene grid to not show
    m_grid->setVisible(false);
    m_grid->setAnchor(DEFAULT_ANCHOR_GRID);

    // reset gene plotter to visible
    m_gene_plotter->setVisible(true);
    m_gene_plotter->setAnchor(DEFAULT_ANCHOR_GENE);

    // reset legend to visible true
    m_legend->setVisible(false);
    m_legend->setAnchor(DEFAULT_ANCHOR_LEGEND);

    // reset visual modes
    m_ui->actionShow_toggleNormal->setChecked(true);

    // reset show cell tissue
    m_ui->actionShow_showCellTissue->setChecked(true);

    // reset show grids
    m_ui->actionShow_showGrid->setChecked(false);

    // reset genes to show
    m_ui->actionShow_showGenes->setChecked(true);

    // reset genes cut off
    m_ui->actionIndividual_gene_cut_off->setChecked(true);

    // gene controls
    m_geneIntensitySlider->setValue(GENE_INTENSITY_MAX);
    m_geneSizeSlider->setValue(GENE_SIZE_MIN);
    m_geneShapeComboBox->setCurrentIndex(GeneRendererGL::Circle);

    // selection mode
    m_ui->selection->setChecked(false);

    // anchor signals
    m_ui->action_toggleLegendTopRight->setChecked(true);

    // show legend and minimap
    m_ui->actionShow_showLegend->setChecked(false);

    // reset pooling mode
    m_poolingReads->setChecked(true);

    // reset color mode
    m_colorLinear->setChecked(true);

    // restrict interface
    m_ui->actionShow_cellTissueRed->setVisible(!m_dataProxy->getFigureRed().isEmpty()
                                               && !m_dataProxy->getFigureRed().isNull());
}

void CellViewPage::slotSetGeneVisualMode(QAction *action)
{
    const QVariant variant = action->property("mode");
    if (variant.canConvert(QVariant::Int)) {
        const GeneRendererGL::GeneVisualMode mode
            = static_cast<GeneRendererGL::GeneVisualMode>(variant.toInt());
        m_gene_plotter->setVisualMode(mode);
    } else {
        Q_ASSERT("[CellViewPage] Undefined gene visual mode!");
    }
}

void CellViewPage::slotSetLegendAnchor(QAction *action)
{
    const QVariant variant = action->property("mode");
    if (variant.canConvert(QVariant::Int)) {
        const Anchor mode = static_cast<Anchor>(variant.toInt());
        m_legend->setAnchor(mode);
    } else {
        Q_ASSERT("[CellViewPage] Undefined legend anchor!");
    }
}

void CellViewPage::slotGeneShape(int geneShape)
{
    const GeneRendererGL::GeneShape shape = static_cast<GeneRendererGL::GeneShape>(geneShape);
    Q_ASSERT(!m_gene_plotter.isNull());
    m_gene_plotter->setShape(shape);
}

// input is expected to be >= 1 and <= 10
void CellViewPage::slotGeneIntensity(int geneIntensity)
{
    Q_ASSERT(geneIntensity >= 1 && geneIntensity <= 10);
    Q_ASSERT(!m_gene_plotter.isNull());
    const float decimal = static_cast<float>(geneIntensity) / 10;
    m_gene_plotter->setIntensity(decimal);
}

// input is expected to be >= 5 and <= 30
void CellViewPage::slotGeneSize(int geneSize)
{
    Q_ASSERT(geneSize >= 5 && geneSize <= 30);
    Q_ASSERT(!m_gene_plotter.isNull());
    const float decimal = static_cast<float>(geneSize) / 10;
    m_gene_plotter->setSize(decimal);
}
