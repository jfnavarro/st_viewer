/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "BooleanItemDelegate.h"

#include <QApplication>
#include <QPainter>
#include <QCheckBox>
#include <QMouseEvent>
#include <QItemSelectionModel>

#include <QTableView>

BooleanItemDelegate::BooleanItemDelegate(QTableView *table) :
  QStyledItemDelegate(table), m_tableView(table)
{

}

BooleanItemDelegate::~BooleanItemDelegate()
{

}

QWidget* BooleanItemDelegate::createEditor(QWidget* parent,
                                           const QStyleOptionViewItem& option,
                                           const QModelIndex& index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    QCheckBox *box = new QCheckBox(parent);
    return box;
}

void BooleanItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    const bool value = index.model()->data(index).toBool();
    QCheckBox *box = qobject_cast<QCheckBox*>(editor);
    box->setChecked(value);
}

void BooleanItemDelegate::updateEditorGeometry(QWidget* editor,
                                               const QStyleOptionViewItem& option,
                                               const QModelIndex& index) const
{
    Q_UNUSED(index);
    const QRect rect = option.rect;
    editor->setGeometry(rect);
}

void BooleanItemDelegate::setModelData(QWidget* editor,
                                       QAbstractItemModel* model, const QModelIndex& index) const
{
    QCheckBox *box = qobject_cast<QCheckBox*>(editor);
    const bool value = box->isChecked();
    model->setData(index, value);
}

void BooleanItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                const QModelIndex& index) const
{
    const bool checked = index.model()->data(index, Qt::DisplayRole).toBool();
    QStyleOptionButton check_box_style_option;
    check_box_style_option.state |= QStyle::State_Enabled;
    if (checked) {
        check_box_style_option.state |= QStyle::State_On;
    } else {
        check_box_style_option.state |= QStyle::State_Off;
    }
    check_box_style_option.rect = BooleanItemDelegate::checkBoxRect(option);
    QApplication::style()->drawControl(QStyle::CE_CheckBox, &check_box_style_option, painter);
}

// Maybe we should use unamed namespaces instead of the static keyword
// It was recommended here http://stackoverflow.com/a/558210/757777

static void selectIndicesInRow(const QModelIndex &index, QItemSelectionModel::SelectionFlags selectionFlags,  QItemSelectionModel *selectionModel) {
    // The GenesTableView contains 3 columns. The variable "index" corresponds to the middle column.
    selectionModel->select(index, selectionFlags);
    selectionModel->select(index.sibling(index.row(),0), selectionFlags);
    selectionModel->select(index.sibling(index.row(),2), selectionFlags);
}

// This function checks the selection state of "index". Then is sets the opposite selection state
// for the whole row where index resides.

static void toggleSelectionInRow(const QModelIndex &index, QTableView *tableView) {
    Q_ASSERT(tableView);
    auto selectionModel = tableView->selectionModel();
    QModelIndexList list = selectionModel->selectedRows(1);
    if (list.contains(index)) {
        selectIndicesInRow(index,  QItemSelectionModel::Deselect, selectionModel);
    } else {
        selectIndicesInRow(index,  QItemSelectionModel::Select, selectionModel);
    }
}

bool BooleanItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* model,
                                      const QStyleOptionViewItem& option, const QModelIndex& index)
{
    if (event->type() == QEvent::MouseButtonRelease ||
        event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent *mouse_event = dynamic_cast<QMouseEvent*>(event);
        if (mouse_event->button() != Qt::LeftButton ||
            !BooleanItemDelegate::checkBoxRect(option).contains(mouse_event->pos())) {
            return false;
        }
        if (event->type() == QEvent::MouseButtonDblClick) {
            return true;
        }
    } else if (event->type() == QEvent::KeyPress) {
        if (dynamic_cast<QKeyEvent*>(event)->key() != Qt::Key_Space &&
            dynamic_cast<QKeyEvent*>(event)->key() != Qt::Key_Select) {
            return false;
        }
    } else {
        return false;
    }
    const bool checked = index.model()->data(index, Qt::DisplayRole).toBool();

    // Clicking in a checkbox should not change the selection so we change it back
    toggleSelectionInRow(index, m_tableView);
    return model->setData(index, !checked, Qt::EditRole);
}

const QRect BooleanItemDelegate::checkBoxRect(const QStyleOptionViewItem &view_item_style_options)
{
    QStyleOptionButton check_box_style_option;
    QRect check_box_rect = QApplication::style()->subElementRect(
                               QStyle::SE_CheckBoxIndicator,
                               &check_box_style_option);
    QPoint check_box_point(
        view_item_style_options.rect.x() +
        view_item_style_options.rect.width() / 2 -
        check_box_rect.width() / 2,
        view_item_style_options.rect.y() +
        view_item_style_options.rect.height() / 2 -
        check_box_rect.height() / 2);

    return QRect(check_box_point, check_box_rect.size());
}
