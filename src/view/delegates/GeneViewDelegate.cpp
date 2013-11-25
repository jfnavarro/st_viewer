/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "GeneViewDelegate.h"

#include <QItemEditorFactory>
#include <QItemEditorCreatorBase>
#include <QApplication>

#include "view/editors/ColorListEditor.h"

GeneViewDelegate::GeneViewDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
    QItemEditorFactory* factory = new QItemEditorFactory();
    Q_ASSERT(factory != 0);
    QItemEditorCreatorBase* colorListCreator = new QStandardItemEditorCreator<ColorListEditor>();
    factory->registerEditor(QVariant::Color, colorListCreator);
    setItemEditorFactory(factory);
}

GeneViewDelegate::~GeneViewDelegate()
{

}

void GeneViewDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
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
