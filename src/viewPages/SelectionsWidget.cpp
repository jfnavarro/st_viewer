#include "SelectionsWidget.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include "utils/SetTips.h"
#include "viewTables/GeneSelectionTableView.h"
#include "model/GeneSelectionItemModel.h"
#include "utils/Utils.h"

using namespace Globals;

SelectionsWidget::SelectionsWidget(QWidget* parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_geneSelectionFilterLineEdit(nullptr)
    , m_selections_tableview(nullptr)
{
    QVBoxLayout* selectionLayout = new QVBoxLayout();
    selectionLayout->setSpacing(0);
    selectionLayout->setContentsMargins(10, 10, 10, 10);
    QHBoxLayout* selectionBottonsLayout = new QHBoxLayout();
    selectionBottonsLayout->setSpacing(0);
    selectionBottonsLayout->setContentsMargins(0, 5, 0, 5);

    m_geneSelectionFilterLineEdit = new QLineEdit(this);
    m_geneSelectionFilterLineEdit->setFixedSize(CELL_PAGE_SUB_MENU_LINE_EDIT_SIZE);
    m_geneSelectionFilterLineEdit->setClearButtonEnabled(true);
    m_geneSelectionFilterLineEdit->setStyleSheet(CELL_PAGE_SUB_MENU_LINE_EDIT_STYLE);
    setToolTipAndStatusTip(tr("Search by gene name"), m_geneSelectionFilterLineEdit);
    selectionBottonsLayout->addWidget(m_geneSelectionFilterLineEdit);
    selectionBottonsLayout->setAlignment(m_geneSelectionFilterLineEdit, Qt::AlignRight);

    // add buttons layout to main layout
    selectionLayout->addLayout(selectionBottonsLayout);

    // add table to main layout
    m_selections_tableview = new GeneSelectionTableView(this);
    selectionLayout->addWidget(m_selections_tableview);

    // set the main layout
    setLayout(selectionLayout);

    // connections
    connect(m_geneSelectionFilterLineEdit,
            SIGNAL(textChanged(QString)),
            m_selections_tableview,
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

void SelectionsWidget::slotLoadModel(const UserSelection::selectedGenesList& geneList)
{
    // TODO the ideal solution would be to pass here the GeneSelection object
    // instanciated already. Right now, we are creating the GeneSelection
    // object when the users saves the selection and assings a name to it but
    // that is inefficient
    getModel()->loadSelectedGenes(geneList);
}

GeneSelectionItemModel* SelectionsWidget::getModel()
{
    GeneSelectionItemModel* selectionModel
        = qobject_cast<GeneSelectionItemModel*>(getProxyModel()->sourceModel());
    Q_ASSERT(selectionModel);
    return selectionModel;
}

QSortFilterProxyModel* SelectionsWidget::getProxyModel()
{
    QSortFilterProxyModel* selectionsProxyModel
        = qobject_cast<QSortFilterProxyModel*>(m_selections_tableview->model());
    Q_ASSERT(selectionsProxyModel);
    return selectionsProxyModel;
}
