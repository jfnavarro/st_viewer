#include "SelectionsWidget.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QSortFilterProxyModel>

#include "utils/SetTips.h"
#include "viewTables/GeneSelectionTableView.h"
#include "model/GeneSelectionItemModel.h"
#include "SettingsStyle.h"

using namespace Style;

SelectionsWidget::SelectionsWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_geneSelectionFilterLineEdit(nullptr)
    , m_selections_tableview(nullptr)
{
    QVBoxLayout *selectionLayout = new QVBoxLayout();
    selectionLayout->setSpacing(0);
    selectionLayout->setContentsMargins(10, 10, 10, 10);
    QHBoxLayout *selectionBottonsLayout = new QHBoxLayout();
    selectionBottonsLayout->setSpacing(0);
    selectionBottonsLayout->setContentsMargins(0, 5, 0, 5);

    m_geneSelectionFilterLineEdit.reset(new QLineEdit(this));
    m_geneSelectionFilterLineEdit->setFixedSize(CELL_PAGE_SUB_MENU_LINE_EDIT_SIZE);
    m_geneSelectionFilterLineEdit->setClearButtonEnabled(true);
    m_geneSelectionFilterLineEdit->setStyleSheet(CELL_PAGE_SUB_MENU_LINE_EDIT_STYLE);
    setToolTipAndStatusTip(tr("Search by gene name"), m_geneSelectionFilterLineEdit.data());
    selectionBottonsLayout->addWidget(m_geneSelectionFilterLineEdit.data());
    selectionBottonsLayout->setAlignment(m_geneSelectionFilterLineEdit.data(), Qt::AlignRight);

    // add buttons layout to main layout
    selectionLayout->addLayout(selectionBottonsLayout);

    // add table to main layout
    m_selections_tableview.reset(new GeneSelectionTableView(this));
    selectionLayout->addWidget(m_selections_tableview.data());

    // set the main layout
    setLayout(selectionLayout);

    // connections
    connect(m_geneSelectionFilterLineEdit.data(),
            SIGNAL(textChanged(QString)),
            m_selections_tableview.data(),
            SLOT(setGeneNameFilter(QString)));
}

SelectionsWidget::~SelectionsWidget()
{
}

void SelectionsWidget::clear()
{
    m_geneSelectionFilterLineEdit->clearFocus();
    m_geneSelectionFilterLineEdit->clear();

    m_selections_tableview->clearSelection();
    m_selections_tableview->clearFocus();

    getModel()->clear();
}

void SelectionsWidget::slotLoadModel(const UserSelection::geneTotalCountsVector &userSelection)
{
    getModel()->loadSelectedGenes(userSelection);
}

GeneSelectionItemModel *SelectionsWidget::getModel()
{
    GeneSelectionItemModel *selectionModel
        = qobject_cast<GeneSelectionItemModel *>(getProxyModel()->sourceModel());
    Q_ASSERT(selectionModel);
    return selectionModel;
}

QSortFilterProxyModel *SelectionsWidget::getProxyModel()
{
    QSortFilterProxyModel *selectionsProxyModel
        = qobject_cast<QSortFilterProxyModel *>(m_selections_tableview->model());
    Q_ASSERT(selectionsProxyModel);
    return selectionsProxyModel;
}
