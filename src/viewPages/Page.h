/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef PAGE_H
#define PAGE_H

#include <QWidget>

class Error;

// this an abstract class to represent the different pages
// for the tab manager
class Page : public QWidget
{
    Q_OBJECT

public:

    explicit Page(QWidget *parent = 0);
    virtual ~Page() {};

signals:

    void signalError(Error* error);
    void moveToNextPage();
    void moveToPreviousPage();

public slots:

    virtual void onInit() = 0;
    virtual void onEnter() = 0;
    virtual void onExit() = 0;

protected:

    void setWaiting(bool waiting = true);
};

#endif // PAGE_H
