#include "GenesWidget.h"

#include <QMenu>
#include <QVBoxLayout>
#include <QWidgetAction>
#include <QPushButton>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QAction>
#include <QColorDialog>

#include "viewTables/GenesTableView.h"
#include "model/GeneFeatureItemModel.h"
#include "utils/SetTips.h"
#include "SettingsStyle.h"
#include "SettingsVisual.h"

using namespace Style;

GenesWidget::GenesWidget(QSharedPointer<DataProxy> dataProxy, QWidget *parent)
    : QWidget(parent)
    , m_lineEdit(nullptr)
    , m_genes_tableview(nullptr)
    , m_colorList(nullptr)
    , m_dataProxy(dataProxy)
{
    // one layout for the controls and another for the table
    QVBoxLayout *genesLayout = new QVBoxLayout();
    genesLayout->setSpacing(0);
    genesLayout->setContentsMargins(10, 10, 10, 10);
    QHBoxLayout *geneListLayout = new QHBoxLayout();
    geneListLayout->setSpacing(0);
    geneListLayout->setContentsMargins(0, 5, 0, 5);

    // add separation between buttons
    geneListLayout->addSpacing(10);

    QPushButton *showSelectedButton = new QPushButton(this);
    configureButton(showSelectedButton,
                    QIcon(QStringLiteral(":/images/visible.png")),
                    tr("Show selected genes"));
    geneListLayout->addWidget(showSelectedButton);
    // add separation
    geneListLayout->addSpacing(CELL_PAGE_SUB_MENU_BUTTON_SPACE);

    QPushButton *hideSelectedButton = new QPushButton(this);
    configureButton(hideSelectedButton,
                    QIcon(QStringLiteral(":/images/nonvisible.png")),
                    tr("Hide selected genes"));
    geneListLayout->addWidget(hideSelectedButton);
    // add separation
    geneListLayout->addSpacing(CELL_PAGE_SUB_MENU_BUTTON_SPACE);

    QPushButton *selectionAllButton = new QPushButton(this);
    configureButton(selectionAllButton,
                    QIcon(QStringLiteral(":/images/select-all.png")),
                    tr("Select all genes"));
    geneListLayout->addWidget(selectionAllButton);
    // add separation
    geneListLayout->addSpacing(CELL_PAGE_SUB_MENU_BUTTON_SPACE);

    QPushButton *selectionClearAllButton = new QPushButton(this);
    configureButton(selectionClearAllButton,
                    QIcon(QStringLiteral(":/images/unselect-all.png")),
                    tr("Deselect all genes"));
    geneListLayout->addWidget(selectionClearAllButton);
    // add separation
    geneListLayout->addSpacing(CELL_PAGE_SUB_MENU_BUTTON_SPACE);

    QPushButton *showColorButton = new QPushButton(this);
    configureButton(showColorButton,
                    QIcon(QStringLiteral(":/images/select-color.png")),
                    tr("Set color of selected genes"));
    // show color button will open up a color selector
    m_colorList.reset(new QColorDialog(Visual::DEFAULT_COLOR_GENE, this));
    m_colorList->setOption(QColorDialog::DontUseNativeDialog, true);
    geneListLayout->addWidget(showColorButton);
    // add separation
    geneListLayout->addSpacing(CELL_PAGE_SUB_MENU_BUTTON_SPACE);

    m_lineEdit.reset(new QLineEdit(this));
    m_lineEdit->setClearButtonEnabled(true);
    m_lineEdit->setFixedSize(CELL_PAGE_SUB_MENU_LINE_EDIT_SIZE);
    m_lineEdit->setStyleSheet(CELL_PAGE_SUB_MENU_LINE_EDIT_STYLE);
    setToolTipAndStatusTip(tr("Search by gene name"), m_lineEdit.data());
    geneListLayout->addWidget(m_lineEdit.data());
    geneListLayout->setAlignment(m_lineEdit.data(), Qt::AlignRight);

    // add actions menu to main layout
    genesLayout->addLayout(geneListLayout);

    // create genes table
    m_genes_tableview.reset(new GenesTableView(this));
    // add table to main layout
    genesLayout->addWidget(m_genes_tableview.data());

    // set main layout
    setLayout(genesLayout);

    // connections
    connect(showSelectedButton, SIGNAL(clicked(bool)), this, SLOT(slotShowAllSelected()));
    connect(hideSelectedButton, SIGNAL(clicked(bool)), this, SLOT(slotHideAllSelected()));
    connect(selectionAllButton, SIGNAL(clicked(bool)), m_genes_tableview.data(), SLOT(selectAll()));
    connect(selectionClearAllButton,
            SIGNAL(clicked(bool)),
            m_genes_tableview.data(),
            SLOT(clearSelection()));
    connect(showColorButton, &QPushButton::clicked, [=] {
        m_colorList->show();
        m_colorList->raise();
        m_colorList->activateWindow();
    });
    connect(m_colorList.data(), &QColorDialog::colorSelected, [=]() {
        slotSetColorAllSelected(m_colorList->currentColor());
    });
    connect(m_lineEdit.data(),
            SIGNAL(textChanged(QString)),
            m_genes_tableview.data(),
            SLOT(setGeneNameFilter(QString)));
    connect(getModel(),
            SIGNAL(signalSelectionChanged(DataProxy::GeneList)),
            this,
            SIGNAL(signalSelectionChanged(DataProxy::GeneList)));
    connect(getModel(),
            SIGNAL(signalColorChanged(DataProxy::GeneList)),
            this,
            SIGNAL(signalColorChanged(DataProxy::GeneList)));
    connect(getModel(),
            SIGNAL(signalCutOffChanged(DataProxy::GenePtr)),
            this,
            SIGNAL(signalCutOffChanged(DataProxy::GenePtr)));
}

GenesWidget::~GenesWidget()
{
}

void GenesWidget::clear()
{
    m_lineEdit->clearFocus();
    m_lineEdit->clear();

    m_genes_tableview->clearSelection();
    m_genes_tableview->clearFocus();

    getModel()->clearGenes();

    m_colorList->setCurrentColor(Visual::DEFAULT_COLOR_GENE);
}

void GenesWidget::updateModelTable()
{
    m_genes_tableview->update();
}

void GenesWidget::configureButton(QPushButton *button, const QIcon &icon, const QString &tooltip)
{
    Q_ASSERT(button != nullptr);
    button->setIcon(icon);
    button->setIconSize(CELL_PAGE_SUB_MENU_ICON_SIZE);
    button->setFixedSize(CELL_PAGE_SUB_MENU_BUTTON_SIZE);
    button->setStyleSheet(CELL_PAGE_SUB_MENU_BUTTON_STYLE);
    button->setCursor(Qt::PointingHandCursor);
    setToolTipAndStatusTip(tooltip, button);
}

void GenesWidget::slotShowAllSelected()
{
    slotSetVisibilityForSelectedRows(true);
}

void GenesWidget::slotHideAllSelected()
{
    slotSetVisibilityForSelectedRows(false);
}

void GenesWidget::slotSetVisibilityForSelectedRows(bool visible)
{
    getModel()->setGeneVisibility(m_genes_tableview->geneTableItemSelection(), visible);
    m_genes_tableview->update();
}

void GenesWidget::slotSetColorAllSelected(const QColor &color)
{
    getModel()->setGeneColor(m_genes_tableview->geneTableItemSelection(), color);
    m_genes_tableview->update();
}

void GenesWidget::slotDatasetOpen(const QString &datasetId)
{
    Q_UNUSED(datasetId);
    const DataProxy::GeneList &geneList = m_dataProxy->getGeneList();
    getModel()->loadGenes(geneList);
}

void GenesWidget::slotDatasetUpdated(const QString &datasetId)
{
    Q_UNUSED(datasetId);
}

void GenesWidget::slotDatasetRemoved(const QString &datasetId)
{
    Q_UNUSED(datasetId);
    clear();
}

GeneFeatureItemModel *GenesWidget::getModel()
{
    GeneFeatureItemModel *geneModel
        = qobject_cast<GeneFeatureItemModel *>(getProxyModel()->sourceModel());
    Q_ASSERT(geneModel);
    return geneModel;
}

QSortFilterProxyModel *GenesWidget::getProxyModel()
{
    QSortFilterProxyModel *proxyModel
        = qobject_cast<QSortFilterProxyModel *>(m_genes_tableview->model());
    Q_ASSERT(proxyModel);
    return proxyModel;
}
