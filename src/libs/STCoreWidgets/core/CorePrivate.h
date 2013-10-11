/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef COREPRIVATE_H
#define COREPRIVATE_H

#include "Core.h"

#define ST_DECLARE_PUBLIC(Class) \
    inline Class* q_func() { return static_cast<Class *>(q_st_ptr); } \
    inline const Class* q_func() const { return static_cast<const Class *>(q_st_ptr); } \
    friend class Class;

//NOTE Core provides an adaptation of the widgets as designed by wwWidgets:
// http://www.wysota.eu.org/wwwidgets/doc/html/index.html
	
// Interface used to create a bridge between the public interface of a class
// and its private counterpart. ObjectPrivate contains a pointer to a
// privatable object and provides functionality for accessing this pointer.
class ObjectPrivate
{

protected:
    
    inline ObjectPrivate(Privatable* priv) : q_st_ptr(priv) { }

    Privatable* q_st_ptr;
};

#endif // COREPRIVATE_H //
