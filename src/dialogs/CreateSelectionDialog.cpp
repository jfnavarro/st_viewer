#include "CreateSelectionDialog.h"

#include "ui_createSelectionDialog.h"

CreateSelectionDialog::CreateSelectionDialog(QWidget* parent, Qt::WindowFlags f)
    : QDialog(parent, f),
      m_ui(new Ui::createSelectionDialog)
{
    m_ui->setupUi(this);

    connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    move(parent->window()->mapToGlobal(parent->window()->rect().center()) -
        mapToGlobal(rect().center()));
}

CreateSelectionDialog::~CreateSelectionDialog()
{
}

const QString CreateSelectionDialog::getName() const
{
    return m_ui->name->text();
}

const QString CreateSelectionDialog::getComment() const
{
    return m_ui->comment->toPlainText();
}

void CreateSelectionDialog::setName(const QString name)
{
    return m_ui->name->setText(name);
}

void CreateSelectionDialog::setComment(const QString comment)
{
    return m_ui->comment->setText(comment);
}
