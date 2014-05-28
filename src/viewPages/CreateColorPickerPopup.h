/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef CREATECOLORPICKERPOPUP_H
#define CREATECOLORPICKERPOPUP_H

class QColor;
class ColorPickerPopup;
class QWidget;

ColorPickerPopup *createColorPickerPopup(const QColor &selectedColor, QWidget *parent);

#endif // CREATECOLORPICKERPOPUP_H
