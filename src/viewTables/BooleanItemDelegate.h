/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef BOOLEANITEMDELEGATE_H
#define BOOLEANITEMDELEGATE_H

#include <QStyledItemDelegate>

// Specialized item delegate to manage the visualizing and modification of
// boolean values in a table view.
class BooleanItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:

    explicit BooleanItemDelegate(QObject* parent = 0);
    virtual ~BooleanItemDelegate();

    QWidget* createEditor(QWidget* parent,
                          const QStyleOptionViewItem& option,
                          const QModelIndex& index) const override;

    void setEditorData(QWidget* editor, const QModelIndex& index) const override;

    void updateEditorGeometry(QWidget* editor,
                              const QStyleOptionViewItem& option,
                              const QModelIndex& index) const override;

    void setModelData(QWidget* editor,
                      QAbstractItemModel* model,
                      const QModelIndex& index) const override;

    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

    bool editorEvent(QEvent* event,
                     QAbstractItemModel* model,
                     const QStyleOptionViewItem& option,
                     const QModelIndex& index) override;

    Q_DISABLE_COPY(BooleanItemDelegate)
};

#endif // BOOLEANITEMDELEGATE_H //
