/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef LINEEDITCLEARPRIVATE_H
#define LINEEDITCLEARPRIVATE_H

#if !defined(ST_NO_LINEEDITBUTTON) && !defined(ST_NO_LINEEDITCLEAR)

#include "LineEditClear.h"

#include "LineEditButtonPrivate.h"

//NOTE Core provides an adaptation of the widgets as designed by wwWidgets:
// http://www.wysota.eu.org/wwwidgets/doc/html/index.html

// Private interface of the LineEditClear class.
class LineEditClearPrivate : public LineEditButtonPrivate
{

public:
    
    explicit LineEditClearPrivate(LineEditClear* pub);

    bool m_visible;

    void _q_textChanged(const QString& text);
    void _q_clearRequested();
    void retranslateUi();

    ST_DECLARE_PUBLIC(LineEditClear);
};

#endif // not ST_NO_LINEEDITBUTTON and not ST_NO_LINEEDITCLEAR //
#endif // LINEEDITCLEARPRIVATE_H //