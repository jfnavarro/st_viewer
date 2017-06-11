#ifndef SELECTIONGENESWIDGET_H
#define SELECTIONGENESWIDGET_H

#include <QWidget>
#include <QPointer>

#include "data/UserSelection.h"

namespace Ui
{
class genesSelectionWidget;
} // namespace Ui

// This widgets is part of the UserSelectionsPage. It shows the list of unique
// genes present in a UserSelection and their aggregated counts.
// It contains a search field to search genes by name.
class SelectionGenesWidget : public QWidget
{
    Q_OBJECT

public:

    explicit SelectionGenesWidget(const UserSelection::GeneListType &genes,
                                  const UserSelection::Matrix &counts,
                                  QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~SelectionGenesWidget();

public slots:

signals:

private:

    // GUI UI object
    QScopedPointer<Ui::genesSelectionWidget> m_ui;


    Q_DISABLE_COPY(SelectionGenesWidget)
};

#endif // SELECTIONGENESWIDGET_H
