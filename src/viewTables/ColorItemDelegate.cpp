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

#include "color/ColorPalette.h"

ColorItemDelegate::ColorItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

ColorItemDelegate::~ColorItemDelegate()
{
}

void ColorItemDelegate::editorFinished(const QColor &)
{
    QWidget *editor = qobject_cast<QWidget *>(sender());
    editor->close();

    //emit commitData(editor);
    //emit closeEditor(editor);
}

void ColorItemDelegate::setModelData(QWidget *editor,
                                    QAbstractItemModel *model, const QModelIndex &index) const
{
    QColorDialog  *colorPickerPopup = qobject_cast<QColorDialog *>(editor);
    Q_ASSERT(colorPickerPopup);
    const QColor color = colorPickerPopup->currentColor();
    if (color.isValid()) {
        const bool res = model->setData(index, color);
        Q_ASSERT(res);
    }
}

void ColorItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QColorDialog  *colorPickerPopup = qobject_cast<QColorDialog *>(editor);
    Q_ASSERT(colorPickerPopup);

    QVariant v = index.model()->data(index, Qt::DisplayRole);
    Q_ASSERT(v.type() == QVariant::Color);
    const QColor color = qvariant_cast<QColor>(v);

    if (color.isValid()) {
        colorPickerPopup->setCurrentColor(color);
    }
}

QWidget* ColorItemDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{
    Q_UNUSED(option);

    QVariant v = index.model()->data(index, Qt::DisplayRole);
    Q_ASSERT(v.type() == QVariant::Color);

    const QColor color = qvariant_cast<QColor>(v);

    QColorDialog *colorPickerPopup = new QColorDialog(color, parent);
    colorPickerPopup->setOption(QColorDialog::NoButtons, true);
    colorPickerPopup->setOption(QColorDialog::DontUseNativeDialog, true);
    colorPickerPopup->setWindowFlags(Qt::Widget);
    //colorPickerPopup->hide();

    //TODO close here or in editor finished
    //connect( colorPickerPopup, &QColorDialog::rejected, [=] { colorPickerPopup->close();  } );
    //connect( colorPickerPopup, &QColorDialog::accept, [=] { colorPickerPopup->close();  } );
    connect( colorPickerPopup, &QColorDialog::currentColorChanged,
        [=]( const QColor& color )
        {
            colorPickerPopup->close();
            Q_UNUSED(color);
            //TODO set color here or in editorFinished
            //editorFinished(color);
            //emit commitData(colorPickerPopup);
            //emit closeEditor(colorPickerPopup);
        });

    return colorPickerPopup;
}

void ColorItemDelegate::updateEditorGeometry(QWidget* editor,
                                            const QStyleOptionViewItem& option,
                                            const QModelIndex& index) const
{
    Q_UNUSED(index);
    // The colorpicker is a bit misplaced. But right now this function doesn't solve the issue
    const QRect rect = option.rect;
    editor->setGeometry(rect);
    //editor->setGeometry( QRect(option.rect.topLeft(), editor->geometry().size() ));
}

void ColorItemDelegate::paint(QPainter* painter,
                             const QStyleOptionViewItem& option,
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
        QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel,
                                           &comboBoxOption, painter);
        break;
    }
    default:
        QStyledItemDelegate::paint(painter, option, index);
    }
}

