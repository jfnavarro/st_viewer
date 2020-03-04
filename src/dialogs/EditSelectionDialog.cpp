#include "EditSelectionDialog.h"

#include <QColorDialog>
#include <QPushButton>
#include "ui_editSelectionDialog.h"

EditSelectionDialog::EditSelectionDialog(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , m_ui(new Ui::editSelectionDialog)
{
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    m_ui->setupUi(this);

    connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    move(parent->window()->mapToGlobal(parent->window()->rect().center())
         - mapToGlobal(rect().center()));
}

EditSelectionDialog::~EditSelectionDialog()
{
}

const QString EditSelectionDialog::getName() const
{
    return m_ui->name->text();
}

const QString EditSelectionDialog::getComment() const
{
    return m_ui->comment->toPlainText();
}

void EditSelectionDialog::setName(const QString name)
{
    return m_ui->name->setText(name);
}

void EditSelectionDialog::setComment(const QString comment)
{
    return m_ui->comment->setText(comment);
}

