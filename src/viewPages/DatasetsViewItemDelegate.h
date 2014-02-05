/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef DATASETSVIEWITEMDELEGATE_H
#define DATASETSVIEWITEMDELEGATE_H

#include <QStyledItemDelegate>

#define LINE_LIMIT 5    //Truncate with ... after a certain amount of word wrapped lines

//NOTE not being used right now (needs to be finished)

// Specialized delegate to manage the visualizing and modification of dataset
// data in a table view.
class DatasetsViewItemDelegate : public QStyledItemDelegate
{

    Q_OBJECT

public:

    explicit DatasetsViewItemDelegate(QObject* parent = 0);
    virtual ~DatasetsViewItemDelegate();

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const;

private:

    void details(QString text, QFont font, const QStyleOptionViewItem &option, int *lineCount, int *widthUsed) const;

    Q_DISABLE_COPY(DatasetsViewItemDelegate)
};

#endif // DATASETSVIEWITEMDELEGATE
