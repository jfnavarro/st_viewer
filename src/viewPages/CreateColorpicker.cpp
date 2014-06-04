#include "CreateColorPickePopup.h"
#include "color/ColorPalette.h"
#include <qtcolorpicker.h>

ColorPickerPopup *createColorPickerPopup(const QColor &selectedColor) {
    ColorPickerPopup *colorPickerPopup = new ColorPickerPopup(2, false, parent);
    HSVPalette palette(parent);
    palette.setSaturation(190); // lighter colors

    const ColorPalette::ColorList colorList = palette.colorList();

    qDebug() << "before inserting colors ........................";

    for (int i = 0; i < colorList.size(); ++i) {
        const ColorPalette::ColorPair pair = colorList[i];
        colorPickerPopup->insertColor(pair.first, pair.second, i);

	qDebug() << "inserting " << pair.first << " " << pair.second;
	/*
        insertItem(i, pair.second);
        setItemData(i, pair.first, Qt::DecorationRole);
	*/
    }
    qDebug() << "after inserting colors ........................";

    /*
    colorPicker->insertColor(QColor("black"), "Black");
    colorPicker->insertColor(QColor("green"), "Green");
    colorPicker->insertColor(QColor("darkGreen"), "Dark green");
    colorPicker->insertColor(QColor("blue"), "Blue");
    colorPicker->insertColor(QColor("darkBlue"), "Dark blue");
    colorPicker->insertColor(QColor("cyan"), "Cyan");
    colorPicker->insertColor(QColor("darkCyan"), "Dark cyan");
    colorPicker->insertColor(QColor("magenta"), "Magenta");
    colorPicker->insertColor(QColor("darkMagenta"), "Dark magenta");
    colorPicker->insertColor(QColor("yellow"), "Yellow");
    colorPicker->insertColor(QColor("grey"), "Grey");
    */
//    ColorListEditor *editor = new ColorListEditor(parent);
//    connect(editor, SIGNAL(currentIndexChanged(int)), this, SLOT(editorFinished(int)));
//    colorPicker->setCurrentColor(color);

    ColorPickerItem *item = colorPickerPopup->find(color);

    Q_ASSERT(item);
    item->setSelected(true);
    return colorPickerPopup;
}

