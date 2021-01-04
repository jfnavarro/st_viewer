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
#include "model/GeneItemModel.h"
#include "data/Dataset.h"

#include "SettingsStyle.h"

using namespace Style;

GenesWidget::GenesWidget(QWidget *parent)
    : QWidget(parent)
    , m_lineEdit(nullptr)
    , m_genes_tableview(nullptr)
    , m_colorList(nullptr)
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
    m_colorList.reset(new QColorDialog(Qt::red, this));
    m_colorList->setOption(QColorDialog::DontUseNativeDialog, true);
    geneListLayout->addWidget(showColorButton);
    // add separation
    geneListLayout->addSpacing(CELL_PAGE_SUB_MENU_BUTTON_SPACE);

    m_lineEdit.reset(new QLineEdit(this));
    m_lineEdit->setClearButtonEnabled(true);
    m_lineEdit->setFixedSize(CELL_PAGE_SUB_MENU_LINE_EDIT_SIZE);
    m_lineEdit->setStyleSheet(CELL_PAGE_SUB_MENU_LINE_EDIT_STYLE);
    m_lineEdit->setToolTip(tr("Search by gene name"));
    m_lineEdit->setStatusTip(tr("Search by gene name"));
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
    connect(showSelectedButton, &QPushButton::clicked, [=]() { slotSetVisible(true); });
    connect(hideSelectedButton, &QPushButton::clicked, [=]() { slotSetVisible(false); });
    connect(selectionAllButton, &QPushButton::clicked,
            m_genes_tableview.data(), &GenesTableView::selectAll);
    connect(selectionClearAllButton,
            &QPushButton::clicked,
            m_genes_tableview.data(),
            &GenesTableView::clearSelection);
    connect(showColorButton, &QPushButton::clicked, [=] {
        m_colorList->show();
        m_colorList->raise();
        m_colorList->activateWindow();
    });
    connect(m_colorList.data(), &QColorDialog::colorSelected, [=]() {
        slotSetColor(m_colorList->currentColor());
    });
    connect(m_lineEdit.data(),
            &QLineEdit::textChanged,
            m_genes_tableview.data(),
            &GenesTableView::setNameFilter);
    connect(m_genes_tableview.data(),
            &GenesTableView::signalUpdated,
            this,
            &GenesWidget::signalUpdated);
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
    m_genes_tableview->getModel()->clear();
    m_colorList->setCurrentColor(Qt::red);
}

void GenesWidget::configureButton(QPushButton *button, const QIcon &icon, const QString &tooltip)
{
    Q_ASSERT(button != nullptr);
    button->setIcon(icon);
    button->setIconSize(CELL_PAGE_SUB_MENU_ICON_SIZE);
    button->setFixedSize(CELL_PAGE_SUB_MENU_BUTTON_SIZE);
    button->setStyleSheet(CELL_PAGE_SUB_MENU_BUTTON_STYLE);
    button->setCursor(Qt::PointingHandCursor);
    button->setToolTip(tooltip);
    button->setStatusTip(tooltip);
}

void GenesWidget::slotSetVisible(bool visible)
{
    m_genes_tableview->getModel()->setVisibility(m_genes_tableview->getItemSelection(), visible);
    m_genes_tableview->update();
    emit signalUpdated();
}

void GenesWidget::slotSetColor(const QColor &color)
{
    m_genes_tableview->getModel()->setColor(m_genes_tableview->getItemSelection(), color);
    m_genes_tableview->update();
    emit signalUpdated();
}

void GenesWidget::slotLoadDataset(const Dataset &dataset)
{
    m_genes_tableview->getModel()->loadData(dataset.data()->genes());
    m_genes_tableview->update();
}
