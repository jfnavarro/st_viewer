/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "SelectionMenuButton.h"
#include "viewPages/GenesTableView.h"

#include <QPushButton>
#include <QMenu>
#include <QIcon>

SelectionMenuButton::SelectionMenuButton(QWidget *parent) :
    QPushButton(parent)
{
    QMenu *menu = new QMenu(parent);
    setMenu(menu);
}

void SelectionMenuButton::init(GenesTableView *genesTableView)
{
    Q_ASSERT(genesTableView);
    menu()->addAction(QIcon(QStringLiteral(":/images/grid-icon-md.png")),
                      tr("Select all rows"), genesTableView, SLOT(selectAll()));
    menu()->addAction(QIcon(QStringLiteral(":/images/grid-icon-md.png")), tr("Deselect all rows"),
                      genesTableView, SLOT(clearSelection()));
}

