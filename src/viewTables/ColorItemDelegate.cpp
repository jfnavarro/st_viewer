/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "ColorItemDelegate.h"

#include <QItemEditorFactory>
#include <QItemEditorCreatorBase>
#include <QApplication>
#include <QDebug>
#include <QColorDialog>
#include <QPalette>

#include "color/ColorListEditor.h"
#include "color/ColorPalette.h"

ColorItemDelegate::ColorItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{

}

ColorItemDelegate::~ColorItemDelegate()
{

}

void ColorItemDelegate::editorFinished(int status)
{
    Q_UNUSED(status);
    QWidget *editor = qobject_cast<QWidget *>(sender());
    Q_ASSERT(editor);
    emit commitData(editor);
    emit closeEditor(editor);
}

void ColorItemDelegate::setModelData(QWidget *editor,
                                    QAbstractItemModel *model, const QModelIndex &index) const
{
    ColorListEditor  *colorListEditor = qobject_cast<ColorListEditor *>(editor);
    Q_ASSERT(colorListEditor);

    const QColor color = colorListEditor->color();
    if (color.isValid()) {
        const bool res = model->setData(index, color);
        Q_UNUSED(res);
    }
}


void ColorItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    ColorListEditor  *colorListEditor = qobject_cast<ColorListEditor *>(editor);
    Q_ASSERT(colorListEditor);

    QVariant v = index.model()->data(index, Qt::DisplayRole);
    Q_ASSERT(v.type() == QVariant::Color);

    const QColor color = qvariant_cast<QColor>(v);
    if (color.isValid()) {
        colorListEditor->setColor(color);
    }
}

QWidget* ColorItemDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{
    Q_UNUSED(option);

    QVariant v = index.model()->data(index, Qt::DisplayRole);
    Q_ASSERT(v.type() == QVariant::Color);

    QColor color = qvariant_cast<QColor>(v);
    ColorListEditor *editor = new ColorListEditor(parent);
    connect(editor, SIGNAL(activated(int)), this, SLOT(editorFinished(int)));
    editor->setColor(color);

    return editor;
}
