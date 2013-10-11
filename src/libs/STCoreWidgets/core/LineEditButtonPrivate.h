/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef LINEEDITBUTTONPRIVATE_H
#define LINEEDITBUTTONPRIVATE_H

#ifndef ST_NO_LINEEDITBUTTON

#include "CorePrivate.h"

#include "LineEditButton.h"

//NOTE Core provides an adaptation of the widgets as designed by wwWidgets:
// http://www.wysota.eu.org/wwwidgets/doc/html/index.html

// Private interface of the LineEditButton class.
class LineEditButtonPrivate : public ObjectPrivate
{
    
public:
    
    explicit LineEditButtonPrivate(LineEditButton* pub);
    void updateButtonPosition(int position);

    QToolButton* m_button;
    LineEditButton::Position m_position;
    bool m_visible;

protected:
    
    ST_DECLARE_PUBLIC(LineEditButton);
};

#endif // not ST_NO_LINEEDITBUTTON //
#endif // LINEEDITBUTTONPRIVATE_H //
