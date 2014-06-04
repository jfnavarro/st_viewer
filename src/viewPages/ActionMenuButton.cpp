/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "ActionMenuButton.h"

#include <QPushButton>
#include <QMenu>
#include <QIcon>
#include <QWidgetAction>

#include <qtcolorpicker.h>
#include "CreateColorPickerPopup.h"
#include "viewPages/CellViewPage.h"

ActionMenuButton::ActionMenuButton(QWidget *parent) :
    QPushButton(parent)
{
    QMenu *menu = new QMenu(parent);
    setMenu(menu);
    setToolTip(tr("Action on selected rows"));
    setText(tr("Action"));
    m_colorPickerPopup = createColorPickerPopup(QColor(), parent);
}

void ActionMenuButton::init(CellViewPage *cellViewPage)
{
    // add actions to act on selected rows

    menu()->addAction(QIcon(QStringLiteral(":/images/grid-icon-md.png")),
                      tr("Show all selected rows"), cellViewPage, SLOT(slotShowAllSelected()));
    menu()->addAction(QIcon(QStringLiteral(":/images/grid-icon-md.png")),
                      tr("Hide all selected rows"), cellViewPage, SLOT(slotHideAllSelected()));
    menu()->addSeparator();
    connect(m_colorPickerPopup, SIGNAL(selected(const QColor &)), cellViewPage, SLOT(slotSetColorAllSelected(const QColor &)));
    QWidgetAction *widgetAction = new QWidgetAction(this);
    widgetAction->setDefaultWidget(m_colorPickerPopup);
    menu()->addAction(tr("Set color of selected:"));
    menu()->addAction(widgetAction);
}
