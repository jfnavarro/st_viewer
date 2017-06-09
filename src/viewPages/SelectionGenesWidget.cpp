#include "SelectionGenesWidget.h"

#include <QSortFilterProxyModel>
#include "SettingsStyle.h"

#include "ui_genesSelectionWidget.h"

using namespace Style;

SelectionGenesWidget::SelectionGenesWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_ui(new Ui::genesSelectionWidget())
{
    m_ui->setupUi(this);
    m_ui->searchField->setClearButtonEnabled(true);
    m_ui->searchField->setFixedSize(CELL_PAGE_SUB_MENU_LINE_EDIT_SIZE);
    m_ui->searchField->setStyleSheet(CELL_PAGE_SUB_MENU_LINE_EDIT_STYLE);
/*
    // settings for the table
    m_ui->tableview->setSortingEnabled(true);
    m_ui->tableview->setShowGrid(true);
    m_ui->tableview->setWordWrap(true);
    m_ui->tableview->setAlternatingRowColors(true);
    m_ui->tableview->sortByColumn(GeneItemModel::Name, Qt::AscendingOrder);

    m_ui->tableview->setFrameShape(QFrame::StyledPanel);
    m_ui->tableview->setFrameShadow(QFrame::Sunken);
    m_ui->tableview->setGridStyle(Qt::SolidLine);
    m_ui->tableview->setCornerButtonEnabled(false);
    m_ui->tableview->setLineWidth(1);

    m_ui->tableview->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ui->tableview->setSelectionMode(QAbstractItemView::MultiSelection);
    m_ui->tableview->setEditTriggers(QAbstractItemView::DoubleClicked);

    m_ui->tableview->horizontalHeader()->setSectionResizeMode(GeneItemModel::Name, QHeaderView::Stretch);
    m_ui->tableview->horizontalHeader()->setSectionResizeMode(GeneItemModel::Color, QHeaderView::Fixed);
    m_ui->tableview->horizontalHeader()->resizeSection(GeneItemModel::Color, 50);
    m_ui->tableview->horizontalHeader()->setSectionResizeMode(GeneItemModel::Show, QHeaderView::Fixed);
    m_ui->tableview->horizontalHeader()->resizeSection(GeneItemModel::Show, 50);
    m_ui->tableview->horizontalHeader()->setSectionResizeMode(GeneItemModel::CutOff, QHeaderView::Fixed);
    m_ui->tableview->horizontalHeader()->resizeSection(GeneItemModel::Show, 50);
    m_ui->tableview->horizontalHeader()->setSortIndicatorShown(true);
    m_ui->tableview->verticalHeader()->hide();

    m_ui->tableview->model()->submit(); // support for caching (speed up)
*/
}

SelectionGenesWidget::~SelectionGenesWidget()
{
}

void SelectionGenesWidget::loaData(const UserSelection::GeneListType &genes,
                                   const UserSelection::Matrix &counts)
{
    Q_UNUSED(genes)
    Q_UNUSED(counts)
}

