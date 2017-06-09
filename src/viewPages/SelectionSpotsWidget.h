#ifndef SELECTIONSPOTSWIDGET_H
#define SELECTIONSPOTSWIDGET_H

#include <QWidget>
#include "data/UserSelection.h"

namespace Ui
{
class spotsSelectionWidget;
} // namespace Ui

class SelectionSpotsWidget : public QWidget
{
    Q_OBJECT

public:

    explicit SelectionSpotsWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~SelectionSpotsWidget();

    void loaData(const UserSelection::SpotListType &spots,
                 const UserSelection::Matrix &counts);
signals:

public slots:

private:

    // GUI UI object
    QScopedPointer<Ui::spotsSelectionWidget> m_ui;

};

#endif // SELECTIONSPOTSWIDGET_H
