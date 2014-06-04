#include "CreateColorPickerPopup.h"

#include <QDebug>
#include "color/ColorPalette.h"
#include <qtcolorpicker.h>

ColorPickerPopup *createColorPickerPopup(const QColor &selectedColor, QWidget *parent)
{
    ColorPickerPopup *colorPickerPopup = new ColorPickerPopup(6, false, parent);
    HSVPalette palette(parent);
    palette.setSaturation(190); // lighter colors
    const ColorPalette::ColorList colorList = palette.colorList();
    for (int i = 0; i < colorList.size(); ++i) {
        const ColorPalette::ColorPair pair = colorList[i];
        colorPickerPopup->insertColor(pair.first, pair.second, i);
    }
    if (selectedColor.isValid()) {
        ColorPickerItem *item = colorPickerPopup->find(selectedColor);
        Q_ASSERT(item);
        item->setSelected(true);
    }
    return colorPickerPopup;
}

