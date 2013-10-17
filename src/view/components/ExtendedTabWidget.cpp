/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QDebug>
#include "utils/DebugHelper.h"

#include <QMetaProperty>
#include <QtGui>
#include <QVBoxLayout>
#include <QAbstractButton>
#include <QApplication>
#include <QStyle>
#include <QStackedWidget>
#include <QIcon>

#include "view/pages/InitPage.h"
#include "view/pages/DatasetPage.h"
#include "view/pages/CellViewPage.h"
#include "view/pages/ExperimentPage.h"

#include "ExtendedTabWidget.h"

bool ExtendedButtonGroup::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::MouseButtonPress) 
    {
        return true;
    } 
    return QButtonGroup::event(event);
}

bool ExtendedButton::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::MouseButtonPress) 
    {
        return true;
    } 
    return QPushButton::event(event);
}

bool ExtendedTabWidget::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::MouseButtonPress) 
    {
        return true;
    } 
    return QWidget::event(event);
}

ExtendedTabWidget::ExtendedTabWidget(QWidget *parent) : QWidget(parent)
{
    
    buttonGroup = new ExtendedButtonGroup;
    
    stackWidget = new QStackedWidget;
    stackWidget->setFrameShape(QFrame::StyledPanel);
    
    buttonLayout = new QVBoxLayout();
    buttonLayout->setSpacing(0);
    
    QVBoxLayout* buttonStretchLayout = new QVBoxLayout();
    buttonStretchLayout->setSpacing(0);
    buttonStretchLayout->addLayout(buttonLayout);
    buttonStretchLayout->addStretch();
    
    layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(buttonStretchLayout);
    layout->addWidget(stackWidget);
    setLayout(layout);
    
    createPages(); //create pages and add them to the tab manager
    createActions(); //create signals - action connections
    
    // enter first pages
    tabChanged(0, -1);
}

ExtendedTabWidget::~ExtendedTabWidget()
{
    // exit last page
    const int index = currentIndex();
    tabChanged(-1, index); 
}

void ExtendedTabWidget::buttonClicked(int index)
{
    ExtendedButton *button = reinterpret_cast<ExtendedButton*>(buttonGroup->button(index));   
    button->setChecked(index == currentIndex());
}

QSize ExtendedTabWidget::sizeHint() const
{
    int xMax=0, yMax=0;
    foreach( QAbstractButton* button, buttonGroup->buttons() )
    {
        xMax = qMax(xMax, button->sizeHint().width());
        yMax = qMax(yMax, button->sizeHint().height());
    }
    return QSize(xMax, yMax);
}

void ExtendedTabWidget::addPage(QWidget *page, const QIcon &icon, const QString &title)
{
    insertPage(count(), page, icon, title);
}

void ExtendedTabWidget::removePage(int index)
{
    QWidget *widget = stackWidget->widget(index);
    stackWidget->removeWidget(widget);
    ExtendedButton *button = reinterpret_cast<ExtendedButton*>(buttonGroup->button(index));  
    buttonLayout->removeWidget(button);
    buttonGroup->removeButton(button);
    delete button;
    setCurrentIndex(0);
}

void ExtendedTabWidget::addTab(QWidget * page, const QString & title)
{ 
    addPage(page, QIcon(), title); 
}

void ExtendedTabWidget::addTab(QWidget * page, const QIcon & icon, const QString & title)
{
    addPage(page, icon, title); 
}

int ExtendedTabWidget::count() const
{
    return stackWidget->count();
}

int ExtendedTabWidget::currentIndex() const
{
    return stackWidget->currentIndex();
}

void ExtendedTabWidget::insertPage(int index, QWidget *page, const QIcon &icon, const QString &title)
{
    
    stackWidget->insertWidget(index, page);
    
    // Set label
    QString label = title;
    if( label.isEmpty() )
    {
        label = tr("Page %1").arg(index);
    }
    
    page->setWindowTitle(label);
    
    // Set icon
    QIcon pix = icon;
    if( pix.isNull() )
    {
        pix = QApplication::style()->standardIcon(QStyle::SP_ArrowUp);   
    }
    
    page->setWindowIcon(pix);
    
    // Add QPushButton
    ExtendedButton* button = new ExtendedButton(pix, label);
    button->setCheckable(true);
    button->setMouseTracking(false);
    button->setFocusPolicy(Qt::NoFocus);
    button->setChecked(count()==1);
    button->setStyleSheet("QPushButton {width: 100px; height: 100px; icon-size: 50px; }");
    buttonGroup->addButton(button, index);
    buttonLayout->addWidget(button);
}

void ExtendedTabWidget::setCurrentIndex(int index)
{
    if( index < 0 || index >= count() )
    {
        index = 0;
    }
    if( index != currentIndex() )
    {
        stackWidget->setCurrentIndex(index);
        buttonGroup->button(index)->setChecked(true);
        emit currentIndexChanged(index);
    }
}

QWidget* ExtendedTabWidget::widget(int index)
{
    return stackWidget->widget(index);
}

int ExtendedTabWidget::indexOf(QWidget* widget)
{
    for( int i=0; i<stackWidget->count(); i++ )
    {
        if( stackWidget->widget(i) == widget )
        {
            return i;
        }
    }
    return -1;
}

bool ExtendedTabWidget::setVisible(QWidget* w, bool b)
{
    int index = indexOf(w);
    
    if( index == -1 ) 
    {
        return false;
    }
    
    if( currentIndex() == index )
    {    
        setCurrentIndex(0);
    }
    
    buttonGroup->button(index)->setVisible(b);
    return true;
}

bool ExtendedTabWidget::setEnabled(QWidget* w, bool b)
{
    int index = indexOf(w);
    
    if( index == -1 ) 
    {    
        return false;
    }
    
    if( currentIndex() == index )
    {    
        setCurrentIndex(0);
    }
    
    buttonGroup->button(index)->setEnabled(b);
    return true;
}

void ExtendedTabWidget::tabChanged(int toIndex, int fromIndex)
{
    qDebug() << QString("[PageContainerWidget] Page: %1 -> %2").arg(fromIndex).arg(toIndex);
    
    QWidget* fromWidget = widget(fromIndex);
    QWidget* toWidget = widget(toIndex);
    
    if (fromWidget != 0)
    {
        QMetaObject::invokeMethod(fromWidget, "onExit");
    }
    
    if (toWidget != 0)
    {
        QMetaObject::invokeMethod(toWidget, "onEnter");
    }
    setCurrentIndex(toIndex);
}

void ExtendedTabWidget::moveToNextPage()
{
    const int index = currentIndex();
    tabChanged(index + 1, index);
}

void ExtendedTabWidget::moveToPreviousPage()
{
    const int index = currentIndex();
    tabChanged(index - 1, index);
}

void ExtendedTabWidget::createPages()
{
    startpage = new InitPage(stackWidget);
    addTab(startpage, QIcon(QStringLiteral(":/images/startpage-icon.png")), "Start");
    
    datasets = new DatasetPage(stackWidget);
    addTab(datasets, QIcon(QStringLiteral(":/images/datasetpage-icon.png")), "Datasets");
    
    cellview = new CellViewPage(stackWidget);
    addTab(cellview, QIcon(QStringLiteral(":/images/gene.png")), "Cell View");
    
    //     experiments = new ExperimentPage(this);
    //     addTab(experiments, QIcon(QStringLiteral(":/images/experimentpage-icon.png")), "Experiments");
}

void ExtendedTabWidget::createActions()
{
    // transition signals
    connect(startpage, SIGNAL(moveToNextPage()), this, SLOT(moveToNextPage()));
    connect(startpage, SIGNAL(moveToPreviousPage()), this, SLOT(moveToPreviousPage()));
    connect(datasets, SIGNAL(moveToNextPage()), this, SLOT(moveToNextPage()));
    connect(datasets, SIGNAL(moveToPreviousPage()), this, SLOT(moveToPreviousPage()));
    connect(cellview, SIGNAL(moveToNextPage()), this, SLOT(moveToNextPage()));
    connect(cellview, SIGNAL(moveToPreviousPage()), this, SLOT(moveToPreviousPage()));
    //connect(experiments, SIGNAL(moveToNextPage()), this, SLOT(moveToNextPage()) );
    //connect(experiments, SIGNAL(moveToPreviousPage()), this, SLOT(moveToPreviousPage()) );
    
    // propagate error signals
    connect(startpage, SIGNAL(signalError(Error*)), this, SIGNAL(signalError(Error*)));
    connect(datasets, SIGNAL(signalError(Error*)), this, SIGNAL(signalError(Error*)));
    connect(cellview, SIGNAL(signalError(Error*)), this, SIGNAL(signalError(Error*)));
    //connect(experiments,SIGNAL(signalError(Error*)), this, SIGNAL(signalError(Error*)) );
    
    // signal to pass the selected dataset id from dataset page to cell view page
    connect(datasets, SIGNAL(datasetSelected(QString)), cellview, SLOT(datasetSelected(QString)) );
}

void ExtendedTabWidget::resetStatus()
{
    qDebug() << "[ExtendedTabWidget] : reseting to start page..";
    for(int index = currentIndex(); index > 0; index--)
    {
        tabChanged(index - 1, index);
    }
}
