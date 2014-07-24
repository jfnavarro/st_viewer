#ifndef EDITDATASETDIALOG_H
#define EDITDATASETDIALOG_H

#include <QDialog>

namespace Ui {
class editDatasetDialog;
} // namespace Ui //

class EditDatasetDialog : public QDialog
{
    Q_OBJECT

public:

    explicit EditDatasetDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~EditDatasetDialog();

    const QString getName() const;
    const QString getComment() const;

    void setName(const QString name);
    void setComment(const QString name);

private:

    Ui::editDatasetDialog *m_ui;

    Q_DISABLE_COPY(EditDatasetDialog)
};

#endif // EDITDATASETDIALOG_H
