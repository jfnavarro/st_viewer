/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "ExtendedTabWidget.h"

#include <QDebug>
#include <QMetaProperty>
#include <QtGui>
#include <QVBoxLayout>
#include <QAbstractButton>
#include <QApplication>
#include <QStyle>
#include <QStackedWidget>
#include <QIcon>

#include "viewPages/InitPage.h"
#include "viewPages/DatasetPage.h"
#include "viewPages/CellViewPage.h"
#include "viewPages/ExperimentPage.h"

bool ExtendedButtonGroup::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::MouseButtonPress) {
        return true;
    }
    return QButtonGroup::event(event);
}

bool ExtendedButton::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::MouseButtonPress) {
        return true;
    }
    return QPushButton::event(event);
}

bool ExtendedTabWidget::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::MouseButtonPress) {
        return true;
    }
    return QWidget::event(event);
}

ExtendedTabWidget::ExtendedTabWidget(QWidget *parent) :
    QWidget(parent),
    startpage(nullptr),
    datasets(nullptr),
    cellview(nullptr),
    experiments(nullptr),
    stackWidget(nullptr),
    buttonGroup(nullptr),
    layout(nullptr),
    buttonLayout(nullptr)
{
    buttonGroup = new ExtendedButtonGroup;

    stackWidget = new QStackedWidget;
    stackWidget->setFrameShape(QFrame::StyledPanel);

    buttonLayout = new QVBoxLayout;
    buttonLayout->setSpacing(0);

    QVBoxLayout  *buttonStretchLayout = new QVBoxLayout;
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

    startpage->deleteLater();
    startpage = nullptr;

    datasets->deleteLater();
    datasets = nullptr;

    cellview->deleteLater();
    cellview = nullptr;

    experiments->deleteLater();
    experiments = nullptr;

    stackWidget->deleteLater();
    stackWidget = nullptr;

    buttonGroup->deleteLater();
    buttonGroup = nullptr;

    layout->deleteLater();
    layout = nullptr;

    buttonLayout->deleteLater();
    buttonLayout = nullptr;
}

QSize ExtendedTabWidget::sizeHint() const
{
    int xMax = 0;
    int yMax = 0;
    foreach(QAbstractButton  *button, buttonGroup->buttons()) {
        xMax = qMax(xMax, button->sizeHint().width());
        yMax = qMax(yMax, button->sizeHint().height());
    }
    return QSize(xMax, yMax);
}

int ExtendedTabWidget::count() const
{
    Q_ASSERT(stackWidget);
    return stackWidget->count();
}

int ExtendedTabWidget::currentIndex() const
{
     Q_ASSERT(stackWidget);
    return stackWidget->currentIndex();
}

void ExtendedTabWidget::insertPage(QWidget *page,
                                   const QIcon &icon, const QString &title)
{
    Q_ASSERT(stackWidget);

    const int index = count();
    stackWidget->insertWidget(index, page);

    // Set label
    QString label = title;
    if (label.isEmpty()) {
        label = tr("Page %1").arg(index);
    }
    page->setWindowTitle(label);

    // Set icon
    QIcon pix = icon;
    if (pix.isNull()) {
        pix = QApplication::style()->standardIcon(QStyle::SP_ArrowUp);
    }
    page->setWindowIcon(pix);

    // Add QPushButton
    ExtendedButton *button = new ExtendedButton(pix, label);
    button->setCheckable(true);
    button->setMouseTracking(false);
    button->setFocusPolicy(Qt::NoFocus);
    button->setChecked(count() == 1);
    button->setStyleSheet("QPushButton {width: 100px; height: 100px; icon-size: 50px; }");

    buttonGroup->addButton(button, index);
    buttonLayout->addWidget(button);
}

void ExtendedTabWidget::setCurrentIndex(int index)
{
    if (index < 0 || index >= count()) {
        index = 0;
    }

    if (index != currentIndex()) {
        stackWidget->setCurrentIndex(index);
        buttonGroup->button(index)->setChecked(true);
        emit currentIndexChanged(index);
    }
}

QWidget* ExtendedTabWidget::widget(int index)
{
    return stackWidget->widget(index);
}

void ExtendedTabWidget::tabChanged(int toIndex, int fromIndex)
{
    qDebug() << QString("[PageContainerWidget] Page: %1 -> %2").arg(fromIndex).arg(toIndex);

    QWidget *fromWidget = widget(fromIndex);
    QWidget *toWidget = widget(toIndex);

    if (fromWidget != 0) {
        QMetaObject::invokeMethod(fromWidget, "onExit", Qt::DirectConnection);
    }

    setCurrentIndex(toIndex);

    if (toWidget != 0) {
        QMetaObject::invokeMethod(toWidget, "onEnter", Qt::DirectConnection);
    }
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
    Q_ASSERT(stackWidget);

    startpage = new InitPage(stackWidget);
    insertPage(startpage, QIcon(QStringLiteral(":/images/startpage-icon.png")), "Start");

    datasets = new DatasetPage(stackWidget);
    insertPage(datasets, QIcon(QStringLiteral(":/images/datasetpage-icon.png")), "Datasets");

    cellview = new CellViewPage(stackWidget);
    insertPage(cellview, QIcon(QStringLiteral(":/images/gene.png")), "Cell View");

    experiments = new ExperimentPage(stackWidget);
    insertPage(experiments, QIcon(QStringLiteral(":/images/experimentpage-icon.png")), "Analysis");
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
    connect(experiments, SIGNAL(moveToNextPage()), this, SLOT(moveToNextPage()));
    connect(experiments, SIGNAL(moveToPreviousPage()), this, SLOT(moveToPreviousPage()));
}

void ExtendedTabWidget::resetStatus()
{
    qDebug() << "[ExtendedTabWidget] : reseting to start page..";
    for (int index = currentIndex(); index > 0; --index) {
        tabChanged(index - 1, index);
    }
}
