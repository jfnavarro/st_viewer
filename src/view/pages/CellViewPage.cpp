/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QDebug>
#include "utils/DebugHelper.h"

#include <QMetaObject>
#include <QMetaMethod>
#include <QString>
#include <QSortFilterProxyModel>
#include <QSpinBox>
#include <QPrintDialog>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QPrinter>
#include <QWidgetAction>
#include <QToolButton>
#include <QSpinBox>
#include <QComboBox>
#include <QColorDialog>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>

#include "controller/io/GeneXMLExporter.h"
#include "controller/io/GeneTXTExporter.h"

#include "view/delegates/BooleanItemDelegate.h"
#include "view/delegates/GeneViewDelegate.h"
#include "utils/Utils.h"

#include "view/controllers/GeneFeatureItemModel.h"

#include "view/components/SelectionDialog.h"
#include "view/components/openGL/GraphicsViewGL.h"
#include "view/components/openGL/GraphicsSceneGL.h"
#include "view/components/openGL/ImageItemGL.h"
#include "view/components/openGL/GenePlotterGL.h"
#include "view/components/openGL/HeatMapLegendGL.h"
#include "view/components/QDoubleSlider.h"

#include "CellViewPage.h"
#include "ui_cellview.h"

CellViewPage::CellViewPage(QWidget *parent)
    : Page(parent), m_selection_mode(false),
    scene(0), cell_tissue(0), gene_plotter_gl(0),
    colorDialog_genes(0), colorDialog_grid(0),
    m_heatmap(0)
{
    onInit();
}

CellViewPage::~CellViewPage()
{
    finalizeGL();
    
    if(colorDialog_grid)
    {
        delete colorDialog_grid;
    }
    colorDialog_grid = 0;
    
    if(colorDialog_genes)
    {
        delete colorDialog_genes;
    }
    colorDialog_genes = 0;
    
    delete ui;
}

void CellViewPage::onInit()
{
    DEBUG_FUNC_NAME
   
    ui = new Ui::CellView;
    ui->setupUi(this);

    // model view for genes list selector
    BooleanItemDelegate* booleanItemDelegate = new BooleanItemDelegate(this);
    GeneViewDelegate* geneViewDelegate = new GeneViewDelegate(this);
    
    geneModel = new GeneFeatureItemModel(this);
    QSortFilterProxyModel* sortProxyModel = new QSortFilterProxyModel(this);
    sortProxyModel->setSourceModel(geneModel);
    sortProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    sortProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    sortProxyModel->setFilterRegExp(QRegExp("(ambiguous)*|(^[0-9])*", Qt::CaseInsensitive)); //I do not want to show ambiguous genes or numbers
    
    ui->genes_treeview->setModel(sortProxyModel);
    ui->genes_treeview->setItemDelegateForColumn(GeneFeatureItemModel::Show, booleanItemDelegate);
    ui->genes_treeview->setItemDelegateForColumn(GeneFeatureItemModel::Color, geneViewDelegate);
    ui->genes_treeview->resizeColumnsToContents();
    ui->genes_treeview->horizontalHeader()->sectionResizeMode(QHeaderView::Interactive);
    ui->genes_treeview->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->genes_treeview->setEditTriggers(QAbstractItemView::AllEditTriggers);
    ui->genes_treeview->setSortingEnabled(true);
    ui->genes_treeview->sortByColumn(0, Qt::AscendingOrder);
    ui->genes_treeview->setSelectionMode(QAbstractItemView::SingleSelection); 
    ui->genes_treeview->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->genes_treeview->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    ui->genes_treeview->verticalHeader()->hide();
    ui->genes_treeview->model()->submit(); //support for caching (speed up)
    #if QT_VERSION >= 0x050200    
    //gene search displays a clear button
    ui->lineEdit->setClearButtonEnabled(true);
    #endif
    //mouse tracking could cause problems on OSX
    #if defined Q_OS_MAC
    ui->lineEdit->setMouseTracking(false);
    ui->lineEdit->setFocusPolicy(Qt::ClickFocus);
    ui->genes_treeview->setFocusPolicy(Qt::ClickFocus);
    ui->genes_treeview->setMouseTracking(false);
    ui->selectAllGenes->setFocusPolicy(Qt::ClickFocus);
    ui->selectAllGenes->setMouseTracking(false);
    //some OSX and optimization variables
    setAttribute(Qt::WA_MacShowFocusRect,false); 
    setAttribute(Qt::WA_MacOpaqueSizeGrip,false); 
    setAttribute(Qt::WA_MacNormalSize,false); 
    setAttribute(Qt::WA_MacVariableSize,true); 
    setAttribute(Qt::WA_OpaquePaintEvent,false); 
    setAttribute(Qt::WA_PaintOnScreen,false);   
    setAttribute(Qt::WA_LayoutOnEntireRect,false); 
    setAttribute(Qt::WA_LayoutUsesWidgetRect,false); 
    setAttribute(Qt::WA_Mapped,false); 
    setAttribute(Qt::WA_NoSystemBackground,false); 
    #endif
   
    // color dialogs
    colorDialog_grid = new QColorDialog(Globals::color_grid); // it should not inherits from this class
    colorDialog_grid->setOption(QColorDialog::DontUseNativeDialog,true); //OSX native color dialog gives problems
    colorDialog_genes = new QColorDialog(Globals::color_gene); // it should not inherits from this class
    colorDialog_genes->setOption(QColorDialog::DontUseNativeDialog,true); //OSX native color dialog gives problems
   
    // selection dialog
    selectionDialog = new SelectionDialog(this);

    createActions(); //create actions
    createToolBar(); //create tool bar and add it
    createConnections(); //create connections
    
    initGLModel();
    initGLConnections();
}

void CellViewPage::onEnter()
{
    DEBUG_FUNC_NAME
    
    resetActionStates();
    
    // initialize gui elements
    initGLView();

    DataProxy *dataProxy = DataProxy::getInstance();
    DataProxy::HitCountPtr hitCount = dataProxy->getHitCount(dataProxy->getSelectedDataset());
    Q_ASSERT_X(hitCount, "Cell View", "HitCountPtr is NULL");
    
    // load gui elements
    gene_plotter_gl->reset();
    //gene_plotter_gl->setHitCount(hitCount->min(), hitCount->max(), hitCount->sum());
    gene_plotter_gl->updateChipSize();
    gene_plotter_gl->updateGeneData();
    gene_plotter_gl->updateTransformation();
    //this will make the gene visual data to be regenerated
    gene_plotter_gl->setGeneLowerLimit(hitCount->min());
    gene_plotter_gl->setGeneUpperLimit(hitCount->max());

    //heat map
    m_heatmap->setHitCountLimits(hitCount->min(), hitCount->max(), hitCount->sum());
    m_heatmap->setLowerLimit(hitCount->min());
    m_heatmap->setUpperLimit(hitCount->max());
    
    slotLoadCellFigure();
}

void CellViewPage::onExit()
{
    DEBUG_FUNC_NAME
    ui->lineEdit->clearFocus();
    ui->genes_treeview->clearFocus();
    ui->selectAllGenes->clearFocus();
}

void CellViewPage::wheelEvent(QWheelEvent* event)
{
    event->ignore();
}

void CellViewPage::createActions()
{    
    actionShow_showGrid = new QAction(QIcon(QStringLiteral(":/images/grid-icon-md.png")), tr("Show Grid"), this);
    actionShow_showGrid->setCheckable(true);
    actionShow_showGenes = new QAction(QIcon(QStringLiteral(":/images/genes.png")), tr("Show &Genes"), this);
    actionShow_showGenes->setCheckable(true);
    
    actionZoom_zoomIn = new QAction(QIcon(QStringLiteral(":/images/Zoom-In-icon.png")), tr("Zoom &In"), this);
    actionZoom_zoomOut = new QAction(QIcon(QStringLiteral(":/images/Zoom-Out-icon.png")), tr("Zoom &Out"), this);
    
    actionShow_cellTissueBlue = new QAction(QIcon(QStringLiteral(":/images/blue-icon.png")), tr("Load &Blue Cell Tissue"), this);
    actionShow_cellTissueBlue->setCheckable(true);
    actionShow_cellTissueRed = new QAction(QIcon(QStringLiteral(":/images/red-icon.png")), tr("Load &Red Cell Tissue"), this);
    actionShow_cellTissueRed->setCheckable(true);
    actionShow_showCellTissue = new QAction(QIcon(QStringLiteral(":/images/biology.png")), tr("Show Cell &Tissue"), this);
    actionShow_showCellTissue->setCheckable(true);
    
    actionNavigate_goBack = new QAction(QIcon(QStringLiteral(":/images/back.png")), tr("Go Back"), this);
    actionNavigate_goBack->setAutoRepeat(false);

    actionShow_toggleNormal = new QAction(QIcon(QStringLiteral(":/images/blue-icon.png")), tr("Normal Mode"), this);
    actionShow_toggleNormal->setCheckable(true);
    actionShow_toggleNormal->setProperty("mode", GenePlotterGL::NormalMode);

    actionShow_toggleDynamicRange = new QAction(QIcon(QStringLiteral(":/images/dynamicrange.png")), tr("Dynamic Range Mode"), this);
    actionShow_toggleDynamicRange->setCheckable(true);
    actionShow_toggleDynamicRange->setProperty("mode", GenePlotterGL::DynamicRangeMode);

    actionShow_toggleHeatMap = new QAction(QIcon(QStringLiteral(":/images/heatmap.png")), tr("Heat Map Mode"), this);
    actionShow_toggleHeatMap->setCheckable(true);
    actionShow_toggleHeatMap->setProperty("mode", GenePlotterGL::HeatMapMode);

    actionSave_save = new QAction(QIcon(QStringLiteral(":/images/filesave.png")), tr("Save Cell Tissue"),  this);
    actionSave_print = new QAction(QIcon(QStringLiteral(":/images/printer.png")), tr("Print Cell Tissue"), this);
    
    //selection actions
    actionSelection_toggleSelectionMode = new QAction(QIcon(QStringLiteral(":/images/selection.png")), tr("Toggle Gene Selection Mode"), this);
    actionSelection_toggleSelectionMode->setCheckable(true);
    actionSelection_showSelectionDialog = new QAction(QIcon(QStringLiteral(":/images/reg_search.png")), tr("Select Genes"), this);
    actionExport_selection = new QAction(QIcon(QStringLiteral(":/images/export.png")), tr("Export Selected Genes"), this);
        
    // color dialogs
    actionColor_selectColorGenes = new QAction(QIcon(QStringLiteral(":/images/select-by-color-icon.png")), tr("Choose &Color Genes"), this);
    actionColor_selectColorGrid = new QAction(QIcon(QStringLiteral(":/images/edit_color.png")), tr("Choose Color Grid"), this);   
}

void CellViewPage::createConnections()
{
    DEBUG_FUNC_NAME
    
    // go back signal
    connect(actionNavigate_goBack, SIGNAL(triggered(bool)), this, SIGNAL(moveToPreviousPage()));
    
    // gene model signals
    QSortFilterProxyModel *proxyModel = dynamic_cast<QSortFilterProxyModel*>(ui->genes_treeview->model());
    GeneFeatureItemModel *geneModel = dynamic_cast<GeneFeatureItemModel*> (proxyModel->sourceModel());
    connect(ui->lineEdit, SIGNAL(textChanged(QString)), proxyModel, SLOT(setFilterFixedString(QString)));
    connect(ui->selectAllGenes, SIGNAL(clicked(bool)), geneModel, SLOT(selectAllGenesPressed(bool)));
    connect(colorDialog_genes, SIGNAL(colorSelected(QColor)), geneModel, SLOT(setColorGenes(const QColor&))); //NOTE dangerous (change all??)
    
    // cell tissue
    connect(actionShow_cellTissueBlue, SIGNAL(triggered(bool)), this, SLOT(slotLoadCellFigure()));
    connect(actionShow_cellTissueRed, SIGNAL(triggered(bool)), this, SLOT(slotLoadCellFigure()));
    
    // graphic view signals
    connect(actionZoom_zoomIn, SIGNAL(triggered(bool)), ui->view, SLOT(zoomIn()));
    connect(actionZoom_zoomOut, SIGNAL(triggered(bool)), ui->view, SLOT(zoomOut()));
    
    // print canvas
    connect(actionSave_save,  SIGNAL(triggered(bool)), this, SLOT(slotSaveImage()));
    connect(actionSave_print, SIGNAL(triggered(bool)), this, SLOT(slotPrintImage()));

    // export
    connect(actionExport_selection, SIGNAL(triggered(bool)), this, SLOT(slotExportSelection()));
    
    // selection mode
    connect(actionSelection_toggleSelectionMode, SIGNAL(triggered(bool)), this, SLOT(slotActivateSelection(bool)));
    connect(actionSelection_showSelectionDialog, SIGNAL(triggered(bool)), this, SLOT(slotSelectByRegExp()));
    
    //color selectors
    connect(actionColor_selectColorGenes, SIGNAL(triggered(bool)), this, SLOT(slotLoadColor()));
    connect(actionColor_selectColorGrid,  SIGNAL(triggered(bool)), this, SLOT(slotLoadColor()));
}

void CellViewPage::resetActionStates()
{    
    
    DataProxy *dataProxy = DataProxy::getInstance();
    
    //reset selection mode
    // selection mode
    actionSelection_toggleSelectionMode->setChecked(false);
    slotActivateSelection(false);
    
    // load gene model (need to be done first)
    geneModel->loadGenes(dataProxy->getSelectedDataset());
    
    //reset gene colors
    //TOFIX this seg faults...
    //geneModel->setColorGenes(Qt::red);
    
    //reset color dialogs
    //TOFIX this seg faults on MAC...
    //colorDialog_genes->setCurrentColor(Qt::darkBlue);
    //colorDialog_grid->setCurrentColor(Qt::darkGreen);
    
    // reset visual modes
    actionShow_toggleDynamicRange->setChecked(false);
    actionShow_toggleHeatMap->setChecked(false);
    
    //reset gene list selection status
    ui->selectAllGenes->setChecked(false);
    geneModel->selectAllGenesPressed(false);
    
    // reset cell image to show
    cell_tissue->setVisible(true);
    actionShow_showCellTissue->setChecked(true);
    
    // reset gene grid to not show
    gene_plotter_gl->setGridVisible(false);
    actionShow_showGrid->setChecked(false);
    
    //reset genes to show
    actionShow_showGenes->setChecked(true);
    
    // adapt to hit count
    DataProxy::HitCountPtr hitCount = dataProxy->getHitCount(dataProxy->getSelectedDataset());
    
    QSlider* geneHitsThresholdDown = dynamic_cast<QSlider*>(actionWidget_geneHitsThresholdDown->defaultWidget());
    QSlider* geneHitsThresholdUp = dynamic_cast<QSlider*>(actionWidget_geneHitsThresholdUp->defaultWidget());
    QSlider* geneIntensity = dynamic_cast<QSlider*>(actionWidget_geneIntensity->defaultWidget());
    QSlider* geneSize = dynamic_cast<QSlider*>(actionWidget_geneSize->defaultWidget());
    QComboBox* geneShape = dynamic_cast<QComboBox*>(actionWidget_geneShape->defaultWidget());
    
    geneHitsThresholdDown->setMinimum(hitCount->min());
    geneHitsThresholdDown->setMaximum(hitCount->max());
    geneHitsThresholdDown->setValue(hitCount->min());
    geneHitsThresholdDown->setTickInterval(hitCount->max() / 10);
    
    geneHitsThresholdUp->setMinimum(hitCount->min());
    geneHitsThresholdUp->setMaximum(hitCount->max());
    geneHitsThresholdUp->setValue(hitCount->max());
    geneHitsThresholdUp->setTickInterval(hitCount->max() / 10);

    geneIntensity->setValue(Globals::gene_intensity);
    geneSize->setValue(Globals::gene_size);
    geneShape->setCurrentIndex(Globals::gene_shape);
    
    // restrict interface
    DataProxy::UserPtr current_user = dataProxy->getUser();
    actionGroup_cellTissue->setVisible((current_user->role() == Globals::ROLE_CM));
}

void CellViewPage::createToolBar()
{   
    //TODO default values should be placed in Globals
    
    toolBar = new QToolBar();
    
    toolBar->setIconSize(QSize(35,35));
    toolBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    toolBar->addAction(actionNavigate_goBack);
    toolBar->addAction(actionZoom_zoomIn);
    toolBar->addAction(actionZoom_zoomOut);
    toolBar->addAction(actionSelection_toggleSelectionMode);
    toolBar->addAction(actionSelection_showSelectionDialog);
    toolBar->addSeparator();
    toolBar->addAction(actionSave_save);
    toolBar->addAction(actionSave_print);
    toolBar->addAction(actionExport_selection);
    toolBar->addSeparator();
    
    // menu gene plotter actions
    menu_genePlotter = new QMenu(toolBar);
    menu_genePlotter->setTitle(tr("Gene Plotter"));
    menu_genePlotter->addAction(actionShow_showGrid);
    menu_genePlotter->addAction(actionShow_showGenes);
    menu_genePlotter->addSeparator();
    menu_genePlotter->addAction(actionColor_selectColorGenes);
    menu_genePlotter->addAction(actionColor_selectColorGrid);
    menu_genePlotter->addSeparator();
    actionGroup_toggleVisualMode = new QActionGroup(menu_genePlotter);
    actionGroup_toggleVisualMode->setExclusive(true);
    actionGroup_toggleVisualMode->addAction(actionShow_toggleNormal);
    actionGroup_toggleVisualMode->addAction(actionShow_toggleDynamicRange);
    actionGroup_toggleVisualMode->addAction(actionShow_toggleHeatMap);
    menu_genePlotter->addActions(actionGroup_toggleVisualMode->actions());
    menu_genePlotter->addSeparator();
    
    // transcripts cut off
    QSlider *geneHitsThresholdSelectorDown = new QSlider();
    geneHitsThresholdSelectorDown->setRange(1, 0x7FFFFFFF);
    geneHitsThresholdSelectorDown->setSingleStep(1);
    geneHitsThresholdSelectorDown->setValue(1);
    geneHitsThresholdSelectorDown->setOrientation(Qt::Horizontal);
    geneHitsThresholdSelectorDown->setInvertedAppearance(false);
    geneHitsThresholdSelectorDown->setInvertedControls(false);
    geneHitsThresholdSelectorDown->setTickInterval(10);
    geneHitsThresholdSelectorDown->setTickPosition(QSlider::TicksAbove);
    geneHitsThresholdSelectorDown->setToolTip(tr("Lower limit of the number of transcripts."));
    actionWidget_geneHitsThresholdDown = new QWidgetAction(menu_genePlotter);
    actionWidget_geneHitsThresholdDown->setDefaultWidget(geneHitsThresholdSelectorDown);
    menu_genePlotter->addAction(tr("Transcripts Lower Threshold:"));
    menu_genePlotter->addAction(actionWidget_geneHitsThresholdDown);

    QSlider *geneHitsThresholdSelectorUp = new QSlider();
    geneHitsThresholdSelectorUp->setRange(1, 0x7FFFFFFF);
    geneHitsThresholdSelectorUp->setSingleStep(1);
    geneHitsThresholdSelectorUp->setValue(1);
    geneHitsThresholdSelectorUp->setOrientation(Qt::Horizontal);
    geneHitsThresholdSelectorUp->setInvertedAppearance(false);
    geneHitsThresholdSelectorUp->setInvertedControls(false);
    geneHitsThresholdSelectorUp->setTickInterval(10);
    geneHitsThresholdSelectorUp->setTickPosition(QSlider::TicksAbove);
    geneHitsThresholdSelectorUp->setToolTip(tr("Upper limit of the number of transcripts."));
    actionWidget_geneHitsThresholdUp = new QWidgetAction(menu_genePlotter);
    actionWidget_geneHitsThresholdUp->setDefaultWidget(geneHitsThresholdSelectorUp);
    menu_genePlotter->addAction(tr("Transcripts Upper Threshold:"));
    menu_genePlotter->addAction(actionWidget_geneHitsThresholdUp);
    
    // transcripts intensity
    QSlider *geneIntesitySelector = new QSlider();
    geneIntesitySelector->setRange(Globals::gene_intensity_min, 
                                   Globals::gene_intensity_max);
    geneIntesitySelector->setSingleStep(1);
    geneIntesitySelector->setValue(Globals::gene_intensity);
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
    geneSizeSelector->setValue(Globals::gene_size);
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
    geneShapeSelector->addItem("Circles",Globals::Circle);
    geneShapeSelector->addItem("Crosses",Globals::Cross);
    geneShapeSelector->addItem("Squares",Globals::Square);
    geneShapeSelector->setCurrentIndex(Globals::gene_shape);
    geneShapeSelector->setToolTip(tr("Shape of the genes."));
    actionWidget_geneShape = new QWidgetAction(menu_genePlotter);
    actionWidget_geneShape->setDefaultWidget(geneShapeSelector);
    menu_genePlotter->addAction(tr("Shape:"));
    menu_genePlotter->addAction(actionWidget_geneShape);
    
    QToolButton* toolButtonGene = new QToolButton();
    toolButtonGene->setMenu(menu_genePlotter);
    toolButtonGene->setPopupMode(QToolButton::InstantPopup);
    toolButtonGene->setIcon(QIcon(QStringLiteral(":/images/settings2.png")));
    toolButtonGene->setToolTip(tr("Configuration of Genes"));
    toolButtonGene->setText(tr("Configuration of Genes"));
    toolBar->addWidget(toolButtonGene);
    toolBar->addSeparator();
    
    // cell tissue menu
    menu_cellTissue = new QMenu(toolBar);
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
    toolBar->addWidget(toolButtonCell);
    toolBar->addSeparator();
    
    // add tool bar to the layout
    ui->verticalLayout->insertWidget(0, toolBar);
}

void CellViewPage::initGLView()
{
    ui->view->initGL(scene);
    gene_plotter_gl->initGL();

    // heatmap component
    if (m_heatmap == 0)
    {
        m_heatmap = new HeatMapLegendGL();
        m_heatmap->setTransform(QTransform::fromTranslate(-10.0, 10.0));
        //TODO move the anchor to settings
        m_heatmap->setAnchor(ViewItemGL::NorthEast);
        m_heatmap->setVisible(false);
        ui->view->addViewItem(m_heatmap); //NOTE relinquish ownership
        // connect
        connect(actionShow_toggleHeatMap, SIGNAL(toggled(bool)), m_heatmap, SLOT(setVisible(bool)));
        QSlider *geneHitsThresholdDown = dynamic_cast<QSlider*>(actionWidget_geneHitsThresholdDown->defaultWidget());
        QSlider *geneHitsThresholdUp = dynamic_cast<QSlider*>(actionWidget_geneHitsThresholdUp->defaultWidget());
        connect(geneHitsThresholdDown, SIGNAL(valueChanged(int)), m_heatmap, SLOT(setLowerLimit(int)));
        connect(geneHitsThresholdUp, SIGNAL(valueChanged(int)), m_heatmap, SLOT(setUpperLimit(int)));
    }
    
    m_heatmap->setVisible(false);
}

void CellViewPage::initGLModel()
{
    DEBUG_FUNC_NAME

    // creates graphic canvas scene and view
    scene = new GraphicsSceneGL();
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    
    // cell tissue canvas
    cell_tissue = new ImageItemGL();
    cell_tissue->setZValue(0);
    scene->addItem(cell_tissue); //NOTE relinquish ownership
    
    // gene plotter component
    gene_plotter_gl = new GenePlotterGL();
    gene_plotter_gl->setZValue(1);
    scene->addItem(gene_plotter_gl); //NOTE relinquish ownership
}

void CellViewPage::initGLConnections()
{
    // gene model signals
    QSortFilterProxyModel* proxyModel = dynamic_cast<QSortFilterProxyModel*>(ui->genes_treeview->model());
    GeneFeatureItemModel* geneModel = dynamic_cast<GeneFeatureItemModel*> (proxyModel->sourceModel());

    connect(geneModel, SIGNAL(signalSelectionChanged(DataProxy::GenePtr)),
            gene_plotter_gl, SLOT(updateGeneSelection(DataProxy::GenePtr)));
    connect(geneModel, SIGNAL(signalColorChanged(DataProxy::GenePtr)),
            gene_plotter_gl, SLOT(updateGeneColor(DataProxy::GenePtr)));
    
    // gene plot signals files
    QSlider* geneHitsThresholdDown = dynamic_cast<QSlider*>(actionWidget_geneHitsThresholdDown->defaultWidget());
    QSlider* geneHitsThresholdUp = dynamic_cast<QSlider*>(actionWidget_geneHitsThresholdUp->defaultWidget());
    QSlider* geneIntensity = dynamic_cast<QSlider*>(actionWidget_geneIntensity->defaultWidget());
    QSlider* geneSize = dynamic_cast<QSlider*>(actionWidget_geneSize->defaultWidget());
    QComboBox* geneShape = dynamic_cast<QComboBox*>(actionWidget_geneShape->defaultWidget());
    
    connect(geneHitsThresholdDown, SIGNAL(valueChanged(int)), gene_plotter_gl, SLOT(setGeneLowerLimit(int)));
    connect(geneHitsThresholdUp, SIGNAL(valueChanged(int)), gene_plotter_gl, SLOT(setGeneUpperLimit(int)));
    connect(geneIntensity, SIGNAL(valueChanged(int)), gene_plotter_gl, SLOT(setGeneIntensity(int)));
    connect(geneSize, SIGNAL(valueChanged(int)), gene_plotter_gl, SLOT(setGeneSize(int)));
    connect(geneShape, SIGNAL(currentIndexChanged(int)), gene_plotter_gl, SLOT(setGeneShape(int)));

    connect(actionShow_showGenes, SIGNAL(triggered(bool)), gene_plotter_gl, SLOT(setGeneVisible(bool))) ;

    connect(actionGroup_toggleVisualMode, SIGNAL(triggered(QAction*)), this, SLOT(slotSetGeneVisualMode(QAction*)));

    // grid signals
    connect(colorDialog_grid, SIGNAL(colorSelected(const QColor&)), gene_plotter_gl, SLOT(setGridColor(const QColor&)));
    connect(actionShow_showGrid, SIGNAL(triggered(bool)), gene_plotter_gl, SLOT(setGridVisible(bool)));
    
    // cell tissue canvas
    connect(actionShow_showCellTissue, SIGNAL(triggered(bool)), cell_tissue, SLOT(visible(bool)));
}

void CellViewPage::finalizeGL()
{
    if (scene != 0)
    {
        delete scene;
        scene = 0;
    }
    //NOTE scene will delete its children
}

void CellViewPage::slotLoadCellFigure()
{
    DEBUG_FUNC_NAME
    
    DataProxy* dataProxy = DataProxy::getInstance();
    DataProxy::UserPtr current_user = dataProxy->getUser();
    DataProxy::DatasetPtr dataset = dataProxy->getDatasetById(dataProxy->getSelectedDataset());
    
    // early out
    if (current_user.isNull() || dataset.isNull())
    {
        qDebug() << QString("[CellViewPage] Warning: Invalid user or no data!");
        return;
    }
    
    //TODO refactor: do not expose sender
    bool forceRedFigure = (QObject::sender() == actionShow_cellTissueRed);
    bool forceBlueFigure = (QObject::sender() == actionShow_cellTissueBlue);
    bool defaultRedFigure =
        (current_user->role() == Globals::ROLE_CM)
        && !(dataset->figureStatus() & Dataset::Aligned);
    
    bool loadRedFigure = (defaultRedFigure || forceRedFigure) && !forceBlueFigure;
    QString figureid = (loadRedFigure) ? dataset->figureRed() : dataset->figureBlue();

    QIODevice *device = dataProxy->getFigure(figureid);
    if (figureid.isEmpty())
    {
        qDebug() << QString("[CellViewPage] Warning: Unknown figure id (%1)!").arg(figureid);
        return;
    }

    //update checkboxes
    actionShow_cellTissueBlue->setChecked(!loadRedFigure);
    actionShow_cellTissueRed->setChecked(loadRedFigure);
    
    // block gui elements
    actionGroup_cellTissue->setEnabled(false);
    actionNavigate_goBack->setEnabled(false); 

    loadCellFigureSync(device); //we could use loadCellFigureAsync here TODO : (create a flag for this)
}

void CellViewPage::loadCellFigureAsync(QIODevice* device)
{
    //convert image
    async::ImageRequest *imageRequest = async::ImageProcess::createOpenGLImage(device);
    connect(imageRequest, SIGNAL(signalFinished()), this, SLOT(loadCellFigurePost()));
}

void CellViewPage::loadCellFigureSync(QIODevice* device)
{
    //NOTE this is an ugly hack to load/convert the image synchronously (move this to the ImageProcessing class)
    async::ImageProcess::TransformedImage openglimage = async::ImageProcess::convertToGLFormat(device);
    const QImage image = openglimage.first;
    const QTransform transform = openglimage.second;
    // update image component
    cell_tissue->setImage(image);
    cell_tissue->setTransform(transform, false);
    // update gui
    actionNavigate_goBack->setEnabled(true);
    actionGroup_cellTissue->setEnabled(true);
}

void CellViewPage::slotLoadCellFigurePost()
{
    // recreate variables
    async::ImageRequest *imageRequest = dynamic_cast<async::ImageRequest *>(sender());

    Q_ASSERT_X(imageRequest, "CellView", "Image Request is null!");
    
    const QImage image = imageRequest->image();
    const QTransform transform = imageRequest->transform();
    
    // update image component
    cell_tissue->setImage(image);
    cell_tissue->setTransform(transform, false);
    
    // update gui
    actionNavigate_goBack->setEnabled(true);
    actionGroup_cellTissue->setEnabled(true);
    
    // deallocate request
    imageRequest->deleteLater();
}

void CellViewPage::slotPrintImage()
{
    QPrinter printer;
    printer.setOrientation(QPrinter::Landscape);
    
    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    if (dialog->exec() != QDialog::Accepted)
    {
        return;
    }
    
    QPainter painter(&printer);
    QRect rect = painter.viewport();
    
    QImage image = ui->view->grabPixmapGL();
    QSize size = image.size();
    size.scale(rect.size(), Qt::KeepAspectRatio);
    
    painter.setViewport(rect);
    painter.setWindow(image.rect());
    painter.drawImage(0, 0, image);
}

void CellViewPage::slotSaveImage()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), QDir::homePath(), 
                                                    QString("%1;;%2").arg(tr("JPEG Image Files (*.jpg *.jpeg)")).arg(tr("PNG Image Files (*.png)")));
    // early out
    if (filename.isEmpty())
    {
        return;
    }

    // append default extension
    QRegExp regex("^.*\\.(jpg|jpeg|png)$", Qt::CaseInsensitive);
    if (!regex.exactMatch(filename))
    {
        filename.append(".jpg");
    }
    
    const int quality = 100; //quality format (100 max, 0 min, -1 default)
    const QString format = filename.split(".",QString::SkipEmptyParts).at(1); //get the file extension
    
    QImage image = ui->view->grabPixmapGL();
    if (!image.save(filename,format.toStdString().c_str(),quality))
    {
        QMessageBox::warning(this, tr("Save Image"), tr("Error saving image."));
    }
}

void CellViewPage::slotExportSelection()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Export File"), QDir::homePath(), 
                                                    QString("%1;;%2").arg(tr("Text Files (*.txt)")).arg(tr("XML Files (*.xml)")));
    // early out
    if (filename.isEmpty())
    {
        return;
    }

    // append default extension
    QRegExp regex("^.*\\.(txt|xml)$", Qt::CaseInsensitive);
    if (!regex.exactMatch(filename))
    {
        filename.append(".txt");
    }

    // get selected features and extend with data
    const QList<QString> featureIdList = gene_plotter_gl->selectedFeatureList();
    const DataProxy::FeatureList featureList = lookupFeatures(featureIdList);

    // create export context
    DataProxy *dataProxy = DataProxy::getInstance();
    DataProxy::HitCountPtr hitCount = dataProxy->getHitCount(dataProxy->getSelectedDataset());

    QObject context;
    context.setProperty("hitCountMin", QVariant(hitCount->min()));
    context.setProperty("hitCountMax", QVariant(hitCount->max()));
    context.setProperty("hitCountSum", QVariant(hitCount->sum()));

    QFile textFile(filename);

    QFileInfo info(textFile);
    if (textFile.open(QFile::WriteOnly | QFile::Truncate))
    {
        QObject memoryGuard;
        //TODO move intantiation to factory
        GeneExporter *exporter =
            (QString::compare(info.suffix(), "XML", Qt::CaseInsensitive) == 0) ?
            static_cast<GeneExporter *>(new GeneXMLExporter(&memoryGuard)) :
            static_cast<GeneExporter *>(new GeneTXTExporter(GeneTXTExporter::SimpleFull, GeneTXTExporter::TabDelimited, &memoryGuard));

        exporter->exportItem(&textFile, featureList, context);
    }
    textFile.close();
}

void CellViewPage::slotActivateSelection(bool status)
{
    ui->view->setDragMode(status ? QGraphicsView::RubberBandDrag : QGraphicsView::ScrollHandDrag);
    m_selection_mode = status;
}

void CellViewPage::slotSetGeneVisualMode(QAction *action)
{
    QVariant variant = action->property("mode");
    if (variant.canConvert(QVariant::Int))
    {
        GenePlotterGL::VisualMode mode = static_cast<GenePlotterGL::VisualMode>(variant.toInt());
        gene_plotter_gl->setGeneVisualMode(mode);
    }
    else
    {
        qDebug() << "[CellViewPage] Undefined gene visual mode!";
    }
}

void CellViewPage::slotLoadColor()
{
    if (QObject::sender() == actionColor_selectColorGenes)
    {
        qDebug() << "[Cell View] : open gene color dialog";
        colorDialog_genes->open();
    }
    else if (QObject::sender() == actionColor_selectColorGrid)
    {
        qDebug() << "[Cell View] : open grid color dialog";
        colorDialog_grid->open();
    }
}

void CellViewPage::slotSelectByRegExp()
{
    DataProxy *dataProxy = DataProxy::getInstance();
    DataProxy::GeneList geneList = SelectionDialog::selectGenes(dataProxy->getSelectedDataset(), this);
    gene_plotter_gl->selectAll(geneList);
}

const DataProxy::FeatureList CellViewPage::lookupFeatures(const QList<QString> &featureIdList) const
{
    DataProxy *dataProxy = DataProxy::getInstance();
    DataProxy::FeatureList featureList;
    foreach (const QString &featureId, featureIdList)
    {
        featureList << dataProxy->getFeature(dataProxy->getSelectedDataset(), featureId);
    }
    return featureList;
}
