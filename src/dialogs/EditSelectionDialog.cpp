/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "EditSelectionDialog.h"

#include <QColorDialog>
#include <QPushButton>
#include "ui_editSelectionDialog.h"

EditSelectionDialog::EditSelectionDialog(QWidget* parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , m_ui(new Ui::editSelectionDialog)
{
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    m_ui->setupUi(this);

    // populate type combobox
    // TODO we should use the metatype of the enum to iterate its types
    m_ui->type->addItem("Rubberband", 1);
    m_ui->type->addItem("Lazo", 2);
    m_ui->type->addItem("Segmented", 3);
    m_ui->type->addItem("Console", 4);
    m_ui->type->addItem("Cluster", 5);
    m_ui->type->addItem("Other", 6);

    // create QColor dialog
    m_color = new QColorDialog(this);

    connect(m_ui->color, &QPushButton::clicked, [=] { m_color->show(); });
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

const QColor EditSelectionDialog::getColor() const
{
    return m_color->selectedColor();
}

const QString EditSelectionDialog::getType() const
{
    return m_ui->type->currentText();
}

void EditSelectionDialog::setName(const QString name)
{
    return m_ui->name->setText(name);
}

void EditSelectionDialog::setComment(const QString comment)
{
    return m_ui->comment->setText(comment);
}

void EditSelectionDialog::setColor(const QColor color)
{
    m_color->setCurrentColor(color);
}

void EditSelectionDialog::setType(const QString type)
{
    m_ui->type->setCurrentText(type);
}
