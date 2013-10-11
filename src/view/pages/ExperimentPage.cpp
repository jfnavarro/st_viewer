/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QDebug>
#include "utils/DebugHelper.h"

#include "view/controllers/ExperimentsItemModel.h"

#include "ExperimentPage.h"
#include "ui_experiments.h"

ExperimentPage::ExperimentPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::Experiments)
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
    DEBUG_FUNC_NAME

    /* model view for the experiments */
    //experimentsModel = new ExperimentsItemModel ( );
    //experiments_tableView->setModel ( experimentsModel );
}

void ExperimentPage::onEnter()
{
    DEBUG_FUNC_NAME
}

void ExperimentPage::onExit()
{
    DEBUG_FUNC_NAME
}
