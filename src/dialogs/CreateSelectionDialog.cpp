#include "CreateSelectionDialog.h"

#include "ui_createSelectionDialog.h"

CreateSelectionDialog::CreateSelectionDialog(QWidget* parent, Qt::WindowFlags f)
    : QDialog(parent, f),
      ui(new Ui::createSelectionDialog)
{
    ui->setupUi(this);

    //TODO handle the OK and CANCEL button to close the dialog
}

CreateSelectionDialog::~CreateSelectionDialog()
{
    delete ui;
}

const QString CreateSelectionDialog::getName() const
{
    return ui->name->text();
}

const QString CreateSelectionDialog::getComment() const
{
    return ui->comment->toPlainText();
}
