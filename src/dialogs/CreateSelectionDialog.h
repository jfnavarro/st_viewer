#ifndef CREATESELECTIONDIALOG_H
#define CREATESELECTIONDIALOG_H

#include <memory>
#include <QDialog>

namespace Ui {
class createSelectionDialog;
} // namespace Ui //

// Widget that asks the user for the newly created selection name and comments
class CreateSelectionDialog : public QDialog
{
    Q_OBJECT

public:

    explicit CreateSelectionDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~CreateSelectionDialog();

    const QString getName() const;
    const QString getComment() const;

    void setName(const QString name);
    void setComment(const QString name);

private:

    std::unique_ptr<Ui::createSelectionDialog> m_ui;

    Q_DISABLE_COPY(CreateSelectionDialog)
};

#endif // CREATESELECTIONDIALOG_H
