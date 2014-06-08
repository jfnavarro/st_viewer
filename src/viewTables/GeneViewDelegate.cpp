/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GeneViewDelegate.h"

#include <QItemEditorFactory>
#include <QItemEditorCreatorBase>
#include <QApplication>
#include <qtcolorpicker.h>
#include <QDebug>

#include "color/ColorPalette.h"

GeneViewDelegate::GeneViewDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void GeneViewDelegate::editorFinished(const QColor &)
{
    QWidget *editor = qobject_cast<QWidget *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}

void GeneViewDelegate::setModelData(QWidget *editor,
                                    QAbstractItemModel *model, const QModelIndex &index) const
{
    ColorPickerPopup  *colorPickerPopup = qobject_cast<ColorPickerPopup *>(editor);
    Q_ASSERT(colorPickerPopup);
    const QColor color = colorPickerPopup->lastSelected();
    if (color.isValid()) {
        const bool res = model->setData(index, color);
        Q_ASSERT(res);
    }
}

void GeneViewDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    ColorPickerPopup  *colorPickerPopup = qobject_cast<ColorPickerPopup *>(editor);
    Q_ASSERT(colorPickerPopup);

    QVariant v = index.model()->data(index, Qt::DisplayRole);
    Q_ASSERT(v.type() == QVariant::Color);
    const QColor color = qvariant_cast<QColor>(v);

    if (color.isValid()) {
        ColorPickerItem *item = colorPickerPopup->find(color);
        Q_ASSERT(item);
        item->setSelected(true);
    }
}

QWidget* GeneViewDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem & /*option*/,
                                        const QModelIndex &index) const
{
    QVariant v = index.model()->data(index, Qt::DisplayRole);
    Q_ASSERT(v.type() == QVariant::Color);
    const QColor color = qvariant_cast<QColor>(v);
    ColorPickerPopup *colorPickerPopup = ColorPicker::createColorPickerPopup(color, parent);
    connect(colorPickerPopup, SIGNAL(selected(const QColor &)), this, SLOT(editorFinished(const QColor &)));
    return colorPickerPopup;
}

void GeneViewDelegate::updateEditorGeometry(QWidget* editor,
                                            const QStyleOptionViewItem& option,
                                            const QModelIndex& index) const
{
    Q_UNUSED(index);
    // The colorpicker is a bit misplaced. But right now this function doesn't solve the issue
    //    const QRect rect = option.rect;
    //    editor->setGeometry(rect);
    qDebug() << option.rect;
    editor->setGeometry( QRect(option.rect.topLeft(), editor->geometry().size() ));
}

GeneViewDelegate::~GeneViewDelegate()
{
}

void GeneViewDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                             const QModelIndex& index) const
{
    QVariant v = index.model()->data(index, Qt::DisplayRole);
    switch (v.type()) {
    case QVariant::Color: {
        // retrieve color and generate pixmap
        QColor color = qvariant_cast<QColor>(v);
        QPixmap pixmap(10, 10);
        pixmap.fill(color);
        // set style options for ombo box and assign icon from pixmap
        QStyleOptionComboBox comboBoxOption;
        comboBoxOption.rect = option.rect;
        comboBoxOption.state = option.state | QStyle::State_Enabled;
        comboBoxOption.currentIcon = QIcon(pixmap);
        comboBoxOption.iconSize = QSize(10, 10);
        comboBoxOption.editable = false;
        // draw
        QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel, &comboBoxOption, painter);
        break;
    }
    default:
        QStyledItemDelegate::paint(painter, option, index);
    }
}

