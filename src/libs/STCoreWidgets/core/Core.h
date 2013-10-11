/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef CORE_H
#define CORE_H

class ObjectPrivate;

//NOTE Core provides an adaptation of the widgets as designed by wwWidgets:
// http://www.wysota.eu.org/wwwidgets/doc/html/index.html

// Core class represents a class interface privatable. Uses same approach as
// QT internal structure separating public interface from private. Redefining
// some of QT's functionality since it isn't exposed for extension by default.
class Privatable
{

protected:
    
    Privatable(ObjectPrivate* priv);
    ~Privatable();
    ObjectPrivate* d_st_ptr;

};

#define ST_DECLARE_PRIVATE(Class) \
    inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(d_st_ptr); } \
    inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(d_st_ptr); } \
    friend class Class##Private;

#endif // CORE_H //
