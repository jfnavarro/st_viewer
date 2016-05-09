#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <memory>
#include <QDialog>
#include <QPointer>

class QCompleter;
class QString;

namespace Ui
{
class LogIn;
} // namespace Ui //

// Login type widget to ask the user to input login name and password
// Usernames are stored in cache
class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~LoginDialog();

    // clear off the fields
    void clear();

    // Sets the proposed username, that can come for instance
    void setUsername(const QString &username);
    const QString getCurrentUser() const;

    // Sets the current password to propose to the user for the login.
    void setPassword(const QString &password);
    const QString getCurrentPassword() const;

signals:

    // A signal emitted when the login is performed.
    void acceptLogin(const QString &, const QString &);

    // A signal emitted when the login is closed
    void exitLogin();

public slots:

    // A slot to adjust the emitting of the signal.
    void slotAcceptLogin();

protected:
    void keyPressEvent(QKeyEvent *e) override;

private:
    // init graphic stuff
    void setUpGUI();

    // load users from qsettings
    void loadUsers();

    // save users from qsettings
    void saveUsers();

    QScopedPointer<Ui::LogIn> m_ui;
    QScopedPointer<QCompleter> m_completer;

    Q_DISABLE_COPY(LoginDialog)
};

#endif // LOGINDIALOG_H
