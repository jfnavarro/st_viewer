/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "LoginDialog.h"

#include <QDebug>
#include "utils/DebugHelper.h"

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

LoginDialog::LoginDialog(QDialog *parent): QDialog(parent),ui(0)
{
    
    //init UI
    ui = new Ui::LogIn();
    ui->setupUi(this);

    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), QApplication::desktop()->availableGeometry()));
    //load users and create completer
    loadUsers();
    //set the completer to the username field
    ui->username->setCompleter(m_completer);
    
    // connects slots
    //TODO user QDialog signals instead
    connect(ui->buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SIGNAL(exitLogin()));
    connect(ui->buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(slotAcceptLogin()));
}

LoginDialog::~LoginDialog()
{
    //elements are children of main layout
    
    //save users 
    saveUsers();
    
    delete m_completer;
    delete ui;
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
    QSettings settings;
    QStringList userlist = settings.value(Globals::SettingsUsers,QStringList()).toStringList();
    QSet<QString> stringSet = QSet<QString>::fromList(userlist);
    m_completer = new QCompleter(stringSet.toList());
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