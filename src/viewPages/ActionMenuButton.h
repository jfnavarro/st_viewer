/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef ACTIONMENUBUTTON_H
#define ACTIONMENUBUTTON_H

#include <QPushButton>

class CellViewPage;
class ColorPickerPopup;

class ActionMenuButton : public QPushButton
{
    Q_OBJECT

public:

    explicit ActionMenuButton(QWidget *parent = 0);
    void init(CellViewPage *cellViewPage);

private slots:

private:

    ColorPickerPopup *m_colorPickerPopup;

    Q_DISABLE_COPY(ActionMenuButton)
};

#endif // ACTIONMENUBUTTON_H
