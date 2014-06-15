/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef COLORPALETTE_H
#define COLORPALETTE_H

#include <QObject>
#include <QPair>
#include <QList>
#include <QColor>

// ColorPalette represents a set of colours, or a palette, and is
// primarily used as a data loader class.
class ColorPalette : public QObject
{
public:
    explicit ColorPalette(QObject* parent = 0);
    virtual ~ColorPalette();

    typedef QPair<QColor, QString> ColorPair;
    typedef QList<ColorPair> ColorList;

    virtual const ColorList colorList() const = 0;
    virtual const ColorList colorList(const int count) const = 0;
};

// StandardPalette loads the standard colours as defined by QT.
class StandardPalette : public ColorPalette
{
public:
    explicit StandardPalette(QObject* parent = 0);
    virtual ~StandardPalette();

    virtual const ColorList colorList() const;
    virtual const ColorList colorList(const int count) const = 0;

private:
    ColorList m_colorList;
};

// HSVPalette is a simple palette using the HSV color space to
// generate colors
class HSVPalette : public ColorPalette
{

public:

    static const int DEFAULT_SATURATION;
    static const int DEFAULT_VALUE;
    static const int DEFAULT_COUNT;

    HSVPalette(QObject* parent = 0);
    virtual ~HSVPalette();

    void setSaturation(const int saturation);
    void setValue(const int value);

    virtual const ColorList colorList() const;
    virtual const ColorList colorList(const int count) const;

private:

    int m_saturation;
    int m_value;
};

class ColorPickerPopup;
//TODO move this somewhere else,
namespace color {

    ColorPickerPopup *createColorPickerPopup(const QColor &selectedColor,
                                                    QWidget *parent = 0);
}

#endif // COLORPALETTE_H //
