/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "color/ColorPalette.h"

#include <QDebug>
#include <qtcolorpicker.h>

ColorPalette::ColorPalette(QObject* parent)
    : QObject(parent)
{

}
ColorPalette::~ColorPalette()
{

}

StandardPalette::StandardPalette(QObject* parent)
    : ColorPalette(parent)
{
    const QStringList colorNameList = QColor::colorNames();
    foreach(const QString colorName, colorNameList) {
        m_colorList << ColorPair(QColor(colorName), colorName);
    }
}
StandardPalette::~StandardPalette()
{

}
const ColorPalette::ColorList StandardPalette::colorList() const
{
    return m_colorList;
}
const ColorPalette::ColorList StandardPalette::colorList(const int count) const
{
    const int limit = qMin(m_colorList.size(), count);
    return m_colorList.mid(0, limit);
}

const int HSVPalette::DEFAULT_SATURATION = 255;
const int HSVPalette::DEFAULT_VALUE = 255;
const int HSVPalette::DEFAULT_COUNT = 12;

HSVPalette::HSVPalette(QObject* parent)
    : ColorPalette(parent),
      m_saturation(DEFAULT_SATURATION),
      m_value(DEFAULT_VALUE)
{

}
HSVPalette::~HSVPalette()
{

}

void HSVPalette::setSaturation(const int saturation)
{
    m_saturation = saturation;
}
void HSVPalette::setValue(const int value)
{
    m_value = value;
}

const ColorPalette::ColorList HSVPalette::colorList() const
{
    return colorList(DEFAULT_COUNT);
}

const ColorPalette::ColorList HSVPalette::colorList(const int count) const
{
    ColorList colorList;
    const qreal step = qreal(360) / qreal(count);
    for (int i = 0; i < count; ++i) {
        const int hue = int(i * step);
        Q_ASSERT(hue >= 0 && hue <= 359);
        Q_ASSERT(m_saturation >= 0 && m_saturation <= 255);
        Q_ASSERT(m_value >= 0 && m_value <= 255);
        const QColor color = QColor::fromHsv(hue, m_saturation, m_value);
        colorList << ColorPair(color, QString());
    }
    return colorList;
}

namespace color {
ColorPickerPopup *createColorPickerPopup(const QColor &selectedColor, QWidget *parent)
{
    const int width = 6;
    const bool hasColorDialog = false;
    ColorPickerPopup *colorPickerPopup = new ColorPickerPopup(width, hasColorDialog, parent);
    HSVPalette palette(parent);
    palette.setSaturation(255);
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
}
