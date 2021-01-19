#include "GenesTableView.h"

#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QClipboard>
#include <QMenu>
#include <QApplication>
#include <QColorDialog>

#include "model/GeneItemModel.h"

GenesTableView::GenesTableView(QWidget *parent)
    : QTableView(parent)
    , m_sortProxyModel(nullptr)
{
    // model
    GeneItemModel *data_model = new GeneItemModel(this);

    // sorting model
    m_sortProxyModel.reset(new QSortFilterProxyModel(this));
    m_sortProxyModel->setSourceModel(data_model);
    m_sortProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_sortProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    // this is important because sort proxy will use the column 0 by default
    m_sortProxyModel->setFilterKeyColumn(GeneItemModel::Name);
    m_sortProxyModel->setSortRole(Qt::UserRole);
    setModel(m_sortProxyModel.data());

    // settings for the table
    setSortingEnabled(true);
    setShowGrid(true);
    setWordWrap(true);
    setAlternatingRowColors(true);
    sortByColumn(GeneItemModel::Name, Qt::AscendingOrder);
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);
    setGridStyle(Qt::SolidLine);
    setCornerButtonEnabled(false);
    setLineWidth(1);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setEditTriggers(QAbstractItemView::DoubleClicked);

    horizontalHeader()->setSectionResizeMode(GeneItemModel::Name, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(GeneItemModel::Color, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(GeneItemModel::Color, 50);
    horizontalHeader()->setSectionResizeMode(GeneItemModel::Show, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(GeneItemModel::Show, 50);
    horizontalHeader()->setSectionResizeMode(GeneItemModel::Count, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(GeneItemModel::Count, 100);
    horizontalHeader()->setSortIndicatorShown(true);
    verticalHeader()->hide();

    model()->submit(); // support for caching (speed up)

    // allow to show extra functions on the table
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &GenesTableView::customContextMenuRequested,
            this, &GenesTableView::customMenuRequested);
}

GenesTableView::~GenesTableView()
{
}

QItemSelection GenesTableView::getItemSelection() const
{
    const auto &selected = selectionModel()->selection();
    return m_sortProxyModel->mapSelectionToSource(selected);
}

void GenesTableView::setNameFilter(const QString &str)
{
    m_sortProxyModel->setFilterFixedString(str);
}

void GenesTableView::customMenuRequested(const QPoint &pos)
{
    const QModelIndex index = indexAt(pos);
    if (index.isValid()) {
        QMenu *menu = new QMenu(this);
        menu->addAction(new QAction(tr("Copy gene"), this));
        menu->addAction(new QAction(tr("Show/Hide"), this));
        menu->addAction(new QAction(tr("Change color"), this));
        QAction *selection = menu->exec(viewport()->mapToGlobal(pos));
        if (selection != nullptr) {
            const QModelIndex correct_index = m_sortProxyModel->mapToSource(index);
            const QString action_text = selection->text();
            if (action_text == tr("Copy gene")) {
                const QModelIndex new_index = getModel()->index(correct_index.row(), GeneItemModel::Name);
                const QString gene_name = getModel()->data(new_index, Qt::DisplayRole).toString();
                QClipboard *clipboard = QApplication::clipboard();
                clipboard->setText(gene_name);
            } else if (action_text == tr("Show/Hide")) {
                const QModelIndex new_index = getModel()->index(correct_index.row(), GeneItemModel::Show);
                const bool selected = getModel()->data(new_index, Qt::CheckStateRole).toBool();
                getModel()->setVisibility(QItemSelection(correct_index, correct_index), !selected);
                update();
                emit signalUpdated();
            } else if (action_text == tr("Change color")) {
                // launch color selector
                const QColor color = QColorDialog::getColor(Qt::red, this, tr("Gene color"));
                getModel()->setColor(QItemSelection(correct_index, correct_index), color);
                update();
                emit signalUpdated();
            }
        }
    }
}

GeneItemModel *GenesTableView::getModel()
{
    GeneItemModel *geneModel = qobject_cast<GeneItemModel *>(getProxyModel()->sourceModel());
    Q_ASSERT(geneModel);
    return geneModel;
}

QSortFilterProxyModel *GenesTableView::getProxyModel()
{
    QSortFilterProxyModel *proxyModel
        = qobject_cast<QSortFilterProxyModel *>(model());
    Q_ASSERT(proxyModel);
    return proxyModel;
}
