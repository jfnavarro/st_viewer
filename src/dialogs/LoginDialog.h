/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

class QCompleter;
class QString;

namespace Ui
{
class LogIn;
} // namespace Ui //

class LoginDialog : public QDialog
{

    Q_OBJECT

public:

    explicit LoginDialog(QDialog *parent = 0);

    virtual ~LoginDialog();

    // clear off the fields
    void clear();

    // Sets the proposed username, that can come for instance
    void setUsername(const QString& username);
    const QString getCurrentUser() const;

    //Sets the current password to propose to the user for the login.
    void setPassword(const QString& password);
    const QString getCurrentPassword() const;

signals:

    //A signal emitted when the login is performed.
    void acceptLogin(const QString&, const QString&);

    //A signal emitted when the login is closed
    void exitLogin();

public slots:

    //A slot to adjust the emitting of the signal.
    void slotAcceptLogin();

private:

    Ui::LogIn *ui;

    QCompleter* m_completer;

    //init graphic stuff
    void setUpGUI();

    //load users from qsettings
    void loadUsers();

    //save users from qsettings
    void saveUsers();

protected:

    virtual void keyPressEvent(QKeyEvent *e);

     Q_DISABLE_COPY(LoginDialog)
};


#endif // LOGINDIALOG_H
