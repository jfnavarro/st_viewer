/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "LoginDialog.h"

#include <QDebug>
#include "utils/DebugHelper.h"

#include <QHBoxLayout>
#include <QDesktopWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVariant>
#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QSettings>
#include <QKeyEvent>
#include <QCompleter>
#include <QString>

#include "utils/Utils.h"
#include "ui_login.h"

LoginDialog::LoginDialog(QDialog *parent): QDialog(parent),ui(0)
{
    //load users and create completer
    loadUsers();
    
    //inig gui elements
    setUpGUI();
    
    //wrap widget into parent widget
    QHBoxLayout *hlayout = new QHBoxLayout(parent);
    hlayout->addWidget(this);
//     setWindowFlags(Qt::Widget);  
    setFixedSize(360,200);
}

LoginDialog::~LoginDialog()
{
    //elements are children of main layout
    
    //save users 
    saveUsers();
    
    delete m_completer;
    delete ui;
}

void LoginDialog::setUpGUI()
{
    //TODO improve style and layout
    
    // set up the layout
    //TODO should pass QMainWindows as parent and make it modal
    ui = new Ui::LogIn();
    ui->setupUi(this);
    
//     QVBoxLayout *vlayout = new QVBoxLayout(this);
//     vlayout->addLayout(formGridLayout);
//     setLayout(vlayout);
}

void LoginDialog::clear()
{
    ui->username->setText("");
    ui->password->setText("");
    ui->username->clear();
    ui->password->clear();
}

void LoginDialog::setUsername(const QString &username)
{
    ui->username->setText(username);
}

const QString LoginDialog::getCurrentUser() const
{
    return ui->username->text();
}

const QString LoginDialog::getCurrentPassword() const
{
    return ui->password->text();
}

void LoginDialog::loadUsers()
{
    //TODO check if the user is present already
    QSettings settings;
    QStringList userlist = settings.value(Globals::SettingsUsers,QStringList()).toStringList();
    m_completer = new QCompleter(userlist);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
}

void LoginDialog::saveUsers()
{
    //TODO check if the user is present already
    QSettings settings;
    QStringList users = settings.value(Globals::SettingsUsers,QStringList()).toStringList();
    const QString username = ui->username->text();
    qDebug() << "[LoginDialog] Saving User = " << username;
    users.append(username);
    settings.setValue(Globals::SettingsUsers,users);
}


void LoginDialog::setPassword(const QString &password)
{
    ui->password->setText(password);
}

void LoginDialog::slotAcceptLogin()
{
    emit acceptLogin(ui->username->text(),ui->password->text());
    // close this dialog
    close();
}

void LoginDialog::keyPressEvent(QKeyEvent *e)
{
    switch (e->key ()) 
    {
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