/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef MAINMENUBAR_H
#define MAINMENUBAR_H

#include <QMenuBar>
#include "utils/Singleton.h"

class MainMenuBar: public QMenuBar, public Singleton<MainMenuBar>
{

public:
        
    explicit MainMenuBar(QWidget *parent = 0);
    
	virtual ~ MainMenuBar();
    
    void finalize(){};
    
    void init();
};

#endif	/* // MAINMENUBAR_H */
