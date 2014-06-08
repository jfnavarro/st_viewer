/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GENEVIEWDELEGATE_H
#define GENEVIEWDELEGATE_H

#include <QStyledItemDelegate>

// Specialized delegate to manage the visualizing and modification of gene data
// in a table view.

class GeneViewDelegate : public QStyledItemDelegate
{
      Q_OBJECT

public:

    explicit GeneViewDelegate(QObject* parent = 0);
    virtual ~GeneViewDelegate();

    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option,
                       const QModelIndex& index) const;
    virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const;
    virtual void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
                                      const QModelIndex& index) const;
    virtual void setEditorData(QWidget * editor, const QModelIndex & index) const;
    virtual void setModelData(QWidget * editor, QAbstractItemModel * model, 
                              const QModelIndex & index) const;

    Q_DISABLE_COPY(GeneViewDelegate)

private slots:

    void editorFinished(const QColor &);

};

#endif // GENEVIEWDELEGATE_H //
