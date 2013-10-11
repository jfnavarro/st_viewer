/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "CorePrivate.h"

Privatable::Privatable(ObjectPrivate* priv)
{
    d_st_ptr = priv;
}
Privatable::~Privatable()
{
    //NOTE each private object belongs to the privatable object that created it
//     delete d_st_ptr;
}
