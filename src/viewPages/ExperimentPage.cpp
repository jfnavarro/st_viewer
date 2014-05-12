/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QDebug>

#include "model/ExperimentsItemModel.h"

#include "ExperimentPage.h"
#include "ui_experiments.h"

ExperimentPage::ExperimentPage(QWidget *parent)
    : Page(parent), ui(new Ui::Experiments)
{
    ui->setupUi(this);
    onInit();
}

ExperimentPage::~ExperimentPage()
{
    delete ui;
}

void ExperimentPage::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);
}

void ExperimentPage::onInit()
{


}

void ExperimentPage::onEnter()
{

}

void ExperimentPage::onExit()
{

}
