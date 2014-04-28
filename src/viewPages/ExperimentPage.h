/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef EXPERIMENTPAGE_H
#define EXPERIMENTPAGE_H

#include <QWidget>
#include "Page.h"

class ExperimentsItemModel;
namespace Ui
{
class Experiments;
} // namespace Ui //

class ExperimentPage : public Page
{
    Q_OBJECT

public:

    explicit ExperimentPage(QWidget *parent = 0);
    virtual ~ExperimentPage();

protected:

    virtual void mousePressEvent(QMouseEvent* event);
    void setWaiting(bool waiting = true);

public slots:

    void onInit();
    void onEnter();
    void onExit();

private:
    Ui::Experiments *ui;

    Q_DISABLE_COPY(ExperimentPage)
};

#endif  /* EXPERIMENTPAGE_H */

