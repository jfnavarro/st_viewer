/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef MAINSTATUSBAR_H
#define MAINSTATUSBAR_H

#include <QStatusBar>
#include "utils/Singleton.h"

//TODO create and attach progress bar

class MainStatusBar: public QStatusBar, public Singleton<MainStatusBar>
{
    Q_OBJECT
    
public:
    
	explicit MainStatusBar(QWidget * parent = 0);
	virtual ~MainStatusBar();
    
    void finalize(){};
    
    void init(){};
    
public slots:
    
    void onDownloadProgress(const QString &fileName, qint64 bytesReceived, qint64 bytesTotal);
  
};

#endif	/* // MAINSTATUSBAR_H */
