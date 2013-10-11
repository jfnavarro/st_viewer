/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "AboutDialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget* parent, Qt::WindowFlags f)
    : QDialog(parent, f), ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
