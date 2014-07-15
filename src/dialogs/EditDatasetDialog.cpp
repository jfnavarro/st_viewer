#include "EditDatasetDialog.h"

#include "ui_editDatasetDialog.h"

EditDatasetDialog::EditDatasetDialog(QWidget* parent, Qt::WindowFlags f)
    : QDialog(parent, f),
      ui(new Ui::editDatasetDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

EditDatasetDialog::~EditDatasetDialog()
{
    delete ui;
    ui = nullptr;
}

const QString EditDatasetDialog::getName() const
{
    return ui->name->text();
}

const QString EditDatasetDialog::getComment() const
{
    return ui->comment->toPlainText();
}

void EditDatasetDialog::setName(const QString name)
{
    return ui->name->setText(name);
}

void EditDatasetDialog::setComment(const QString comment)
{
    return ui->comment->setText(comment);
}
