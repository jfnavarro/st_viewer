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

BooleanItemDelegate::BooleanItemDelegate(QObject* parent) : QStyledItemDelegate(parent)
{

}

BooleanItemDelegate::~BooleanItemDelegate()
{

}

QWidget* BooleanItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
        const QModelIndex& index) const
{
    QCheckBox* box = new QCheckBox(parent);
    return box;
}
void BooleanItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    bool value = index.model()->data(index).toBool();
    QCheckBox* box = static_cast<QCheckBox*>(editor);
    box->setChecked(value);
}

void BooleanItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
        const QModelIndex& index) const
{
    QRect rect = option.rect;
    editor->setGeometry(rect);
}

void BooleanItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QCheckBox* box = static_cast<QCheckBox*>(editor);
    bool value = box->isChecked();
    model->setData(index, value);
}

void BooleanItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                const QModelIndex& index) const
{
    bool checked = index.model()->data(index, Qt::DisplayRole).toBool();
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

bool BooleanItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option,
                                      const QModelIndex& index)
{
    if ((event->type() == QEvent::MouseButtonRelease) ||
        (event->type() == QEvent::MouseButtonDblClick)) {
        QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
        if (mouse_event->button() != Qt::LeftButton ||
            !BooleanItemDelegate::checkBoxRect(option).contains(mouse_event->pos())) {
            return false;
        }
        if (event->type() == QEvent::MouseButtonDblClick) {
            return true;
        }
    } else if (event->type() == QEvent::KeyPress) {
        if (static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space &&
            static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select) {
            return false;
        }
    } else {
        return false;
    }
    bool checked = index.model()->data(index, Qt::DisplayRole).toBool();
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
