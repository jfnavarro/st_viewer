#ifndef EDITSELECTIONDIALOG_H
#define EDITSELECTIONDIALOG_H

#include <memory>
#include <QDialog>

class QColorDialog;
namespace Ui
{
class editSelectionDialog;
} // namespace Ui //

// Simple dialog that allows to edit some fields in an UserSelection object
class EditSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditSelectionDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~EditSelectionDialog();

    const QString getName() const;
    const QString getComment() const;

    void setName(const QString name);
    void setComment(const QString name);

private:
    QScopedPointer<Ui::editSelectionDialog> m_ui;

    Q_DISABLE_COPY(EditSelectionDialog)
};

#endif // EDITSELECTIONDIALOG_H
