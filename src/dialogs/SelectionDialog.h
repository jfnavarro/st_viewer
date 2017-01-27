#ifndef SELECTIONDIALOG_H
#define SELECTIONDIALOG_H

#include <memory>
#include <QDialog>
#include <QList>

namespace Ui
{
class SelectionDialog;
} // namespace Ui //

// Selection dialog implementing support to select genes by their names
// using regular expressions.
class SelectionDialog : public QDialog
{
    Q_OBJECT

public:

    SelectionDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~SelectionDialog();

    // returns the list of genes found in the reg-exp

    // returns the list of spots found in the reg-exp

signals:

    void signalValidRegExp(bool);

public slots:

    // to get the list of selected genes from the reg-exp
    void accept() override;
    // to validate or not the reg-exp
    void slotValidateRegExp(const QString &pattern);
    // to include or not ambiguos genes in the selection
    void slotIncludeAmbiguous(bool includeAmbiguous);
    // to enable/disable case sensitivigy in the reg-exp
    void slotCaseSensitive(bool caseSensitive);
    // to enable/disable the inclusion of non visible in the selection
    void slotSelectNonVisible(bool selectNonVisible);
    // disable any "accept/yes/apply" type buttons when regexp is invalid
    void slotEnableAcceptAction(bool enableAcceptAction);

private:
    QScopedPointer<Ui::SelectionDialog> m_ui;

    // configuration variables
    bool m_includeAmbiguous;
    bool m_caseSensitive;
    bool m_regExpValid;
    bool m_selectNonVisible;
    QRegExp m_regExp;

    Q_DISABLE_COPY(SelectionDialog)
};

#endif // SELECTIONDIALOG_H //
