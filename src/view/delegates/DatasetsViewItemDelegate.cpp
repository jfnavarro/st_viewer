/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/


#include "DatasetsViewItemDelegate.h"
#include "view/controllers/DatasetItemModel.h"
#include <QPainter>
#include <QApplication>
#include <QTextLayout>

DatasetsViewItemDelegate::DatasetsViewItemDelegate(QObject* parent): QStyledItemDelegate(parent)
{

}

DatasetsViewItemDelegate::~DatasetsViewItemDelegate()
{

}

void DatasetsViewItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, 
                                     const QModelIndex& index) const
{
    const DatasetItemModel *model = qobject_cast<const DatasetItemModel*>(index.model());
    Q_ASSERT(model);
    int col = index.column();
    if(col == DatasetItemModel::Comments)
    {
        int widthUsed, lineCount;
        painter->save();

        QString lines = index.data(Qt::DisplayRole).toString();

        QPalette::ColorGroup group = option.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
        if (group == QPalette::Normal && !(option.state & QStyle::State_Active))
        {
            group = QPalette::Inactive;
        }
        //set pen color depending on behavior
        painter->setFont( QApplication::font() );
        if (option.state & QStyle::State_Selected)
        {
            painter->setPen(option.palette.color(group, QPalette::HighlightedText));
        }
        else
        {
            painter->setPen(option.palette.color(group, QPalette::Text));
        }

        details(lines, QApplication::font(), option, &lineCount, &widthUsed);

        //Word wrap the text, 'elide' it if it goes past a pre-determined maximum
        QString newText = painter->fontMetrics().elidedText(lines, Qt::ElideRight, widthUsed);
        painter->drawText( option.rect, (Qt::TextWrapAnywhere|Qt::TextWordWrap|Qt::AlignTop|Qt::AlignLeft), newText );

        painter->restore();
        
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize DatasetsViewItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    int widthUsed, lineCount;
    const DatasetItemModel *model = qobject_cast<const DatasetItemModel*>(index.model());
    Q_ASSERT(model);
    int col = index.column();
    //Try and word wrap strings
    if(col == DatasetItemModel::Comments)
    {
        //Update the size based on the number of lines (original size of a single line multiplied
        //by the number of lines)
        QString text = index.data(Qt::DisplayRole).toString();
        details(text, QApplication::font(), option, &lineCount, &widthUsed);
        
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        size.setHeight(lineCount * size.height());
        
        return size;
    }
    else
    {
        //Fall back on original size hint of item
        return QStyledItemDelegate::sizeHint(option, index);
    }
}

void DatasetsViewItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                                    const QModelIndex &index ) const
{
    int lineCount, widthUsed;
    const DatasetItemModel *model = qobject_cast<const DatasetItemModel*>(index.model());
    Q_ASSERT(model);
    int col = index.column();
    //Fix editor's geometry and produce the word-wrapped effect for strings...
    if(col == DatasetItemModel::Comments)
    {
        //We need to expand our editor accordingly (not really necessary but looks nice)
        QString text = index.data(Qt::DisplayRole).toString();
        details(text, QApplication::font(), option, &lineCount, &widthUsed);
        
        //Expand
        QSize extraSize = QSize(option.rect.width(), lineCount * option.rect.height());
        QRect extraRect = option.rect;
        extraRect.setSize(extraSize);
        editor->setGeometry(option.rect);
    }
    else
    {
        //Fall back on original update
        QStyledItemDelegate::updateEditorGeometry(editor, option, index);
    }
}

void DatasetsViewItemDelegate::details( QString text, const QFont font, const QStyleOptionViewItem &option,
                                        int *lineCount, int *widthUsed ) const
{
    //Use text layout to word-wrap and provide informmation about line counts and width's
    QTextLayout textLayout(text);
    
    *widthUsed = 0;
    *lineCount = 0;
    textLayout.setFont(font);
    textLayout.beginLayout();
    while (*lineCount < LINE_LIMIT)
    {
        *lineCount = *lineCount + 1;
        QTextLine line = textLayout.createLine();
        if (!line.isValid())
        {
            break;
        }
        line.setLineWidth(option.rect.width());
        *widthUsed = (*widthUsed + line.naturalTextWidth());
    }
    textLayout.endLayout();
    *widthUsed = (*widthUsed + option.rect.width());
}
