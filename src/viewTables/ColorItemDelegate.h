/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef COLORITEMDELEGATE_H
#define COLORITEMDELEGATE_H

#include <QStyledItemDelegate>

// Specialized delegate to manage the visualizing and modification of gene data
// in a table view.

class ColorItemDelegate : public QStyledItemDelegate
{
      Q_OBJECT

public:

    explicit ColorItemDelegate(QObject* parent = 0);
    virtual ~ColorItemDelegate();

    QWidget* createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const override;

    void setEditorData(QWidget * editor, const QModelIndex & index) const override;

    void setModelData(QWidget * editor, QAbstractItemModel * model,
                              const QModelIndex & index) const override;

private slots:

    void editorFinished(int);

private:

    Q_DISABLE_COPY(ColorItemDelegate)

};

#endif // COLORITEMDELEGATE_H //
