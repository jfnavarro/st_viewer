#ifndef EDITDATASETDIALOG_H
#define EDITDATASETDIALOG_H

#include <memory>
#include <QDialog>

namespace Ui
{
class editDatasetDialog;
} // namespace Ui //

// Widget that shows the user the dataset's name and comments fields
class EditDatasetDialog : public QDialog
{
    Q_OBJECT

public:

    explicit EditDatasetDialog(QWidget *parent = nullptr);
    virtual ~EditDatasetDialog();

    const QString getName() const;
    const QString getComment() const;

    void setName(const QString name);
    void setComment(const QString name);

private:

    QScopedPointer<Ui::editDatasetDialog> m_ui;

    Q_DISABLE_COPY(EditDatasetDialog)
};

#endif // EDITDATASETDIALOG_H
