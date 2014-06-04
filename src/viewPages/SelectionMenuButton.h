/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef SELECTIONMENUBUTTON_H
#define SELECTIONMENUBUTTON_H

#include <QPushButton>

class GenesTableView;

class SelectionMenuButton : public QPushButton
{
    Q_OBJECT

public:

    explicit SelectionMenuButton(QWidget *parent = 0);
    void init(GenesTableView *genesTableView);

private slots:

private:

    Q_DISABLE_COPY(SelectionMenuButton)
};

#endif // SELECTIONMENUBUTTON_H
