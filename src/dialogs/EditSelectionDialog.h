/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef EDITSELECTIONDIALOG_H
#define EDITSELECTIONDIALOG_H

#include <memory>
#include <QDialog>

class QColorDialog;
namespace Ui
{
class editSelectionDialog;
} // namespace Ui //

// Simple dialog that allows to edit some fields in of an UserSelection object
// TODO get the types from the UserSelection meta data
class EditSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditSelectionDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~EditSelectionDialog();

    const QString getName() const;
    const QString getComment() const;
    const QColor getColor() const;
    const QString getType() const;

    void setName(const QString name);
    void setComment(const QString name);
    void setColor(const QColor color);
    void setType(const QString type);

private:
    std::unique_ptr<Ui::editSelectionDialog> m_ui;
    QColorDialog* m_color;

    Q_DISABLE_COPY(EditSelectionDialog)
};

#endif // EDITSELECTIONDIALOG_H
