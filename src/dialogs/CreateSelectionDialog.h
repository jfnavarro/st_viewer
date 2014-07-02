#ifndef CREATESELECTIONDIALOG_H
#define CREATESELECTIONDIALOG_H

#include <QDialog>

namespace Ui
{
class createSelectionDialog;
} // namespace Ui //

class CreateSelectionDialog : public QDialog
{
    Q_OBJECT

public:

    explicit CreateSelectionDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~CreateSelectionDialog();

    const QString getName() const;
    const QString getComment() const;

private:

    Ui::createSelectionDialog *ui;

    Q_DISABLE_COPY(CreateSelectionDialog)
};

#endif // CREATESELECTIONDIALOG_H
