/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QWidget>
#include <QString>

class QLineEdit;
class QLabel;
class QDialogButtonBox;
class QCompleter;

class LoginDialog : public QWidget
{

    Q_OBJECT
    
private:

    QLabel* labelUsername;
    
    QLabel* labelPassword;
    
    QLabel* labelHeader;
    
    QLabel* labelError;
    
    QCompleter *completer;
    
    QLineEdit* editPassword;

    QLineEdit* editUSerName;
    
    QDialogButtonBox* buttons;

    //init graphic stuff
    void setUpGUI();
    
    //load users from qsettings
    void loadUsers();
    
    //save users from qsettings
    void saveUsers();
    
    virtual void keyPressEvent(QKeyEvent *e);
    
public:
    
    explicit LoginDialog(QWidget *parent = 0);
    
	virtual ~LoginDialog();
    
    // clear off the fields
    void clear();
    
    // Sets the proposed username, that can come for instance
    void setUsername(const QString& username);
    
    const QString getCurrentUser() const;
    
    //Sets the current password to propose to the user for the login.
    void setPassword(const QString& password);
    
    const QString getCurrentPassword() const;
    
    //Sets the current header of the login form
    void setLabelHeader(const QString& header);
    
    //Sets the error message (if any) of the login form
    void setLabelError(const QString& error);
    
signals:
    
    //A signal emitted when the login is performed.
    void acceptLogin(QString username, QString password);
    
    //A signal emitted when the login is closed
    void exitLogin();
    
public slots:
    
    //A lot to adjust the emitting of the signal.
    void slotAcceptLogin();
    
};


#endif // LOGINDIALOG_H
