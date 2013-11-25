/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "MainStatusBar.h"
#include "utils/Utils.h"

MainStatusBar::MainStatusBar(QWidget * parent): QStatusBar(parent)
{

}

MainStatusBar::~MainStatusBar()
{

}

void MainStatusBar::onDownloadProgress(const QString &fileName, qint64 bytesReceived, qint64 bytesTotal)
{
    showMessage(QString("Downloading %1 (%2/%3)")
                .arg(fileName)
                .arg(Utils::formatStorage(bytesReceived))
                .arg(Utils::formatStorage(bytesTotal)), 1000);
}
