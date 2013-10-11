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

#include "utils/Utils.h"

LoginDialog::LoginDialog(QWidget *parent): QWidget(parent)
{
    labelError = 0;
    labelHeader = 0;
    labelPassword = 0;
    labelUsername = 0;
    completer = 0;
    editPassword = 0;
    buttons = 0;
 
    //load users and create completer
    loadUsers();
    
    //inig gui elements
    setUpGUI();
    
    //wrap widget into parent widget
    QHBoxLayout *hlayout = new QHBoxLayout(parent);
    hlayout->addWidget(this);
    setWindowFlags(Qt::Widget);  
    setFixedSize(360,200);
}

LoginDialog::~LoginDialog()
{
    //elements are children of main layout
    
    //save users 
    saveUsers();
    
    delete labelError;
    delete labelHeader;
    delete labelPassword;
    delete labelUsername;
    delete completer;
    delete editPassword;
    delete buttons;
}

void LoginDialog::setUpGUI()
{
    //TODO improve style and layout
    
    // set up the layout
    QGridLayout* formGridLayout = new QGridLayout();
    
    editUSerName = new QLineEdit();
    editUSerName->setFocusPolicy(Qt::StrongFocus);
    editUSerName->setCompleter(completer);
    
    editPassword = new QLineEdit();
    editPassword->setFocusPolicy(Qt::StrongFocus);
    editPassword->setEchoMode(QLineEdit::Password);
    
    // initialize the labels
    labelUsername = new QLabel();
    labelPassword = new QLabel();
    labelHeader = new QLabel();
    labelError = new QLabel();
    
    labelUsername->setText(tr("Username"));
    labelUsername->setBuddy(editUSerName);
    
    labelPassword->setText(tr( "Password" ));
    labelPassword->setBuddy(editPassword);

    QString labelText = "<P><b><FONT COLOR='black' FONT SIZE = 4>";
    labelText.append(tr("Enter credentials to log in to stVi"));
    labelText.append("</b></P>");
    labelHeader->setAlignment(Qt::AlignCenter);
    labelHeader->setText(labelText);
    labelError->setText("");
    
    // initialize buttons
    //TODO add focus possibiligy to button (tab focus)
    buttons = new QDialogButtonBox();
    buttons->setFocusPolicy(Qt::StrongFocus);
    buttons->addButton(QDialogButtonBox::Ok);
    buttons->addButton(QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText(tr("Login"));
    buttons->button(QDialogButtonBox::Cancel)->setText(tr("Abort"));
    
    // connects slots
    connect(buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SIGNAL(exitLogin()));
    connect(buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(slotAcceptLogin()) );
    
    // place components into the dialog
    formGridLayout->addWidget(labelUsername, 0, 0);
    formGridLayout->addWidget(editUSerName, 0,1);
    formGridLayout->addWidget(labelPassword, 1, 0);
    formGridLayout->addWidget(editPassword, 1, 1);
    formGridLayout->addWidget(buttons, 2, 0, 1, 2);
    formGridLayout->setAlignment(Qt::AlignCenter);
    
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->addWidget(labelHeader);
    vlayout->addWidget(labelError);
    vlayout->addLayout(formGridLayout);
    
    setLayout(vlayout);
}

void LoginDialog::clear()
{
    editUSerName->setText("");
    editPassword->setText("");
    labelError->setText("");
}

void LoginDialog::setUsername(const QString &username)
{
    editUSerName->setText(username);
}

const QString LoginDialog::getCurrentUser() const
{
    return editUSerName->text();
}

const QString LoginDialog::getCurrentPassword() const
{
    return editPassword->text();
}

void LoginDialog::loadUsers()
{
    QSettings settings;
    QStringList userlist = settings.value(Globals::SettingsUsers,QStringList()).toStringList();
    completer = new QCompleter(userlist);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
}

void LoginDialog::saveUsers()
{
    QSettings settings;
    QStringList users = settings.value(Globals::SettingsUsers,QStringList()).toStringList();
    QString username = editUSerName->text();
    qDebug() << "[LoginDialog] Saving User = " << username;
    users.append(username);
    settings.setValue(Globals::SettingsUsers,users);
}


void LoginDialog::setPassword(const QString &password)
{
    editPassword->setText(password);
}

void LoginDialog::setLabelError(const QString& error)
{
    QString labelText = "<P><b><FONT COLOR='#ff0000' FONT SIZE = 2>";
    labelText.append(error);
    labelText.append("</b></P>");
    labelError->setText(labelText);
}

void LoginDialog::setLabelHeader(const QString &header)
{
    labelHeader->setText(header);
}

void LoginDialog::slotAcceptLogin()
{
    QString username = editUSerName->text();
    QString password = editPassword->text();
    
    emit acceptLogin(username,password);

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