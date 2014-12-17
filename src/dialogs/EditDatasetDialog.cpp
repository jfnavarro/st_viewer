#include "EditDatasetDialog.h"

#include "ui_editDatasetDialog.h"


EditDatasetDialog::EditDatasetDialog(QWidget* parent, Qt::WindowFlags f)
    : QDialog(parent, f),
      m_ui(new Ui::editDatasetDialog)
{
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    m_ui->setupUi(this);

    connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    move(parent->window()->mapToGlobal(parent->window()->rect().center()) -
        mapToGlobal(rect().center()));
}

EditDatasetDialog::~EditDatasetDialog()
{
}

const QString EditDatasetDialog::getName() const
{
    return m_ui->name->text();
}

const QString EditDatasetDialog::getComment() const
{
    return m_ui->comment->toPlainText();
}

void EditDatasetDialog::setName(const QString name)
{
    return m_ui->name->setText(name);
}

void EditDatasetDialog::setComment(const QString comment)
{
    return m_ui->comment->setText(comment);
}
