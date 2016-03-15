#include "LoginDialog.h"

#include <QDebug>
#include <QDesktopWidget>
#include <QSettings>
#include <QKeyEvent>
#include <QCompleter>
#include <QString>
#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QSet>

#include "utils/Utils.h"
#include "ui_login.h"

LoginDialog::LoginDialog(QWidget* parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , m_ui(new Ui::LogIn())
    , m_completer(nullptr)
{
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    // init UI
    m_ui->setupUi(this);

    // load users and create completer
    loadUsers();

    // set the completer to the username field
    m_ui->username->setCompleter(m_completer.data());

    // connects slots
    // TODO user QDialog signals instead
    connect(m_ui->buttons->button(QDialogButtonBox::Cancel),
            SIGNAL(clicked()),
            this,
            SIGNAL(exitLogin()));
    connect(m_ui->buttons->button(QDialogButtonBox::Ok),
            SIGNAL(clicked()),
            this,
            SLOT(slotAcceptLogin()));
}

LoginDialog::~LoginDialog()
{
    // save users
    saveUsers();
    // delete completer
    m_completer->deleteLater();
    m_completer = nullptr;
}

void LoginDialog::clear()
{
    m_ui->username->clear();
    m_ui->password->clear();
}

void LoginDialog::setUsername(const QString& username)
{
    m_ui->username->setText(username);
}

const QString LoginDialog::getCurrentUser() const
{
    return m_ui->username->text();
}

const QString LoginDialog::getCurrentPassword() const
{
    return m_ui->password->text();
}

void LoginDialog::loadUsers()
{
    QSettings settings;
    const QStringList userlist
        = settings.value(Globals::SettingsUsers, QStringList()).toStringList();
    QSet<QString> stringSet = QSet<QString>::fromList(userlist);
    m_completer = new QCompleter(stringSet.toList());
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
}

void LoginDialog::saveUsers()
{
    QSettings settings;
    QStringList users = settings.value(Globals::SettingsUsers, QStringList()).toStringList();
    const QString username = m_ui->username->text();
    users.append(username);
    settings.setValue(Globals::SettingsUsers, users);
}

void LoginDialog::setPassword(const QString& password)
{
    m_ui->password->setText(password);
}

void LoginDialog::slotAcceptLogin()
{
    // important to close the dialog before the accept signal es emitted
    close();
    emit acceptLogin(m_ui->username->text(), m_ui->password->text());
}

void LoginDialog::keyPressEvent(QKeyEvent* e)
{
    switch (e->key()) {
    case Qt::Key_Escape:
        exitLogin();
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        slotAcceptLogin();
        break;
    default:
        QWidget::keyPressEvent(e);
    }
}
