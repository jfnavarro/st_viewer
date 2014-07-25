/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef COLORLISTEDITOR_H
#define COLORLISTEDITOR_H

#include <QComboBox>

class ColorPalette;

// A simple color combo box class (not the best solution but works) used to
// provide a drop down color selector. Primarily intended to be used in table
// views as an alternative to edit colors.

class ColorListEditor : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor USER true)

public:

    explicit ColorListEditor(QWidget* widget = 0);
    virtual ~ColorListEditor();

    const QColor color() const;
    void setColor(const QColor& color);

signals:

    void currentIndexChanged(int index);

private:

    void populateList(const ColorPalette* palette);

};

#endif // #define COLORLISTEDITOR_H //
