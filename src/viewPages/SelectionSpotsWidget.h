#ifndef SELECTIONSPOTSWIDGET_H
#define SELECTIONSPOTSWIDGET_H

#include <QWidget>
#include "data/UserSelection.h"

namespace Ui
{
class spotsSelectionWidget;
} // namespace Ui


// This widgets is part of the UserSelectionsPage. It shows the list of unique
// spots present in a UserSelection and their aggregated counts.
// It contains a search field to search genes by name.
class SelectionSpotsWidget : public QWidget
{
    Q_OBJECT

public:

    explicit SelectionSpotsWidget(const UserSelection::STDataFrame &data, QWidget *parent = nullptr);
    virtual ~SelectionSpotsWidget();
signals:

private slots:

    void customMenuRequested(const QPoint &pos);

private:

    // GUI UI object
    QScopedPointer<Ui::spotsSelectionWidget> m_ui;

    Q_DISABLE_COPY(SelectionSpotsWidget)
};

#endif // SELECTIONSPOTSWIDGET_H
