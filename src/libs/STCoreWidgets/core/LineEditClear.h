/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef LINEEDITCLEAR_H
#define LINEEDITCLEAR_H

#if !defined(ST_NO_LINEEDITBUTTON) && !defined(ST_NO_LINEEDITCLEAR)

#include "LineEditButton.h"

//NOTE Core provides an adaptation of the widgets as designed by wwWidgets:
// http://www.wysota.eu.org/wwwidgets/doc/html/index.html

// LineEditClear is a convenience class that extends LineEditButton adding a
// clear button to the line edit field. It preconfigures some of the options
// provided by the parent class simplifying the interface.
class LineEditClearPrivate;
class LineEditClear : public LineEditButton
{
    Q_OBJECT

public:
    
    explicit LineEditClear(QWidget* parent = 0);

protected:
    
    void paintEvent(QPaintEvent* event);
    void changeEvent(QEvent* event);

private slots:
    
    void setButtonVisible(bool vis);

private:
    
    Q_PRIVATE_SLOT(d_func(), void _q_textChanged(const QString& text));
    Q_PRIVATE_SLOT(d_func(), void _q_clearRequested());
	
	Q_DISABLE_COPY(LineEditClear);
    ST_DECLARE_PRIVATE(LineEditClear);
};

#endif // not ST_NO_LINEEDITBUTTON and not ST_NO_LINEEDITCLEAR //
#endif // LINEEDITCLEAR_H //
