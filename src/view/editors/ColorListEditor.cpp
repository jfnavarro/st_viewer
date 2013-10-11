/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QVariant>
#include <QScopedPointer>

#include "ColorListEditor.h"
#include "model/core/ColorPalette.h"

ColorListEditor::ColorListEditor(QWidget* widget)
    : QComboBox(widget)
{
    HSVPalette palette(this);
    palette.setSaturation(190); // lighter colors
    populateList(&palette);
}

ColorListEditor::~ColorListEditor()
{

}

const QColor ColorListEditor::color() const
{
    return qvariant_cast<QColor>(itemData(currentIndex(), Qt::DecorationRole));
}

void ColorListEditor::setColor(const QColor& color)
{
    setCurrentIndex(findData(color, int(Qt::DecorationRole)));
}

void ColorListEditor::populateList(const ColorPalette* palette)
{
    const ColorPalette::ColorList colorList = palette->colorList();

    for (int i = 0; i < colorList.size(); ++i)
    {
        const ColorPalette::ColorPair pair = colorList[i];

        insertItem(i, pair.second);
        setItemData(i, pair.first, Qt::DecorationRole);
    }
}
