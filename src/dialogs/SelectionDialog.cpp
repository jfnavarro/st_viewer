#include "SelectionDialog.h"
#include "ui_selectionConsole.h"

SelectionDialog::SelectionDialog(QWidget *parent,
                                 Qt::WindowFlags f)
    : QDialog(parent, f)
    , m_ui(new Ui::SelectionDialog())
    , m_includeAmbiguous(false)
    , m_caseSensitive(false)
    , m_regExpValid(false)
    , m_selectNonVisible(false)
{

    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    m_ui->setupUi(this);

    // set default state
    slotCaseSensitive(false);
    slotSelectNonVisible(false);
    move(parent->window()->mapToGlobal(parent->window()->rect().center())
         - mapToGlobal(rect().center()));

    // NOTE the connections are made in the UI file

    m_regExp.setPatternSyntax(QRegExp::WildcardUnix);
}

SelectionDialog::~SelectionDialog()
{
}

void SelectionDialog::accept()
{
    // early out, should "never" happen
    if (!m_regExp.isValid()) {
        reject();
        return;
    }

    // and propagate accept call
    QDialog::accept();
}

QRegExp SelectionDialog::getRegExp() const
{
    return m_regExp;
}

bool SelectionDialog::isValid() const
{
    return m_regExpValid;
}

bool SelectionDialog::selectNonVisible() const
{
    return m_selectNonVisible;
}

bool SelectionDialog::caseSensitive() const
{
    return m_caseSensitive;
}

void SelectionDialog::slotValidateRegExp(const QString &pattern)
{
    m_regExp.setPattern(pattern);
    const bool regExpValid = m_regExp.isValid();
    if (regExpValid != m_regExpValid) {
        m_regExpValid = regExpValid;
    }
}

void SelectionDialog::slotSelectNonVisible(bool selectNonVisible)
{
    m_selectNonVisible = selectNonVisible;
    if (m_selectNonVisible != m_ui->checkSelectNonVisible->isChecked()) {
        m_ui->checkSelectNonVisible->setChecked(m_selectNonVisible);
    }
}

void SelectionDialog::slotCaseSensitive(bool caseSensitive)
{
    // toggle case sensitive
    m_regExp.setCaseSensitivity((caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive));
    m_caseSensitive = caseSensitive;
    if (m_caseSensitive != m_ui->checkCaseSense->isChecked()) {
        m_ui->checkCaseSense->setChecked(m_caseSensitive);
    }
}

void SelectionDialog::slotEnableAcceptAction(bool enableAcceptAction)
{
    for (QAbstractButton *button : m_ui->buttonBox->buttons()) {
        const QDialogButtonBox::ButtonRole role = m_ui->buttonBox->buttonRole(button);
        if (role == QDialogButtonBox::AcceptRole || role == QDialogButtonBox::YesRole
            || role == QDialogButtonBox::ApplyRole) {
            button->setEnabled(enableAcceptAction);
        }
    }
}
