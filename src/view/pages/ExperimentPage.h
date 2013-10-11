/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef EXPERIMENTPAGE_H
#define	EXPERIMENTPAGE_H

#include <QWidget>


class ExperimentsItemModel;
namespace Ui
{
    class Experiments;
} // namespace Ui //

class ExperimentPage : public QWidget
{
    Q_OBJECT
    
public:
    
    explicit ExperimentPage(QWidget *parent = 0);
    virtual ~ExperimentPage();

protected:

    virtual void mousePressEvent(QMouseEvent* event);

signals:
    
    void signalError(Error*);
    void moveToNextPage();
    void moveToPreviousPage();

public slots:

    void onInit();
    void onEnter();
    void onExit();

protected:
  
    ExperimentsItemModel *experimentsModel;

private:
    Ui::Experiments *ui;
};

#endif	/* EXPERIMENTPAGE_H */

