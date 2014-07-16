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

ExtendedButtonGroup::ExtendedButtonGroup(QWidget *parent) : QButtonGroup(parent)
{

}

void ExtendedButtonGroup::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
}

void ExtendedButtonGroup::KeyPressEvent(QKeyEvent *event)
{
    event->ignore();
}

void ExtendedButtonGroup::wheelEvent(QWheelEvent *event)
{
    event->ignore();
}

bool ExtendedButtonGroup::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::MouseButtonPress) {
        return true;
    }
    return QButtonGroup::event(event);
}

ExtendedButton::ExtendedButton(QWidget* parent) : QPushButton(parent)
{

}

ExtendedButton::ExtendedButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
{

}

ExtendedButton::ExtendedButton(const QIcon& icon, const QString &text, QWidget *parent)
    : QPushButton(icon, text, parent)
{

}

void ExtendedButton::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
}

void ExtendedButton::KeyPressEvent(QKeyEvent *event)
{
    event->ignore();
}

void ExtendedButton::wheelEvent(QWheelEvent *event)
{
    event->ignore();
}

bool ExtendedButton::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::MouseButtonPress) {
        return true;
    }
    return QPushButton::event(event);
}

ExtendedTabWidget::ExtendedTabWidget(QPointer<DataProxy> dataProxy, QWidget *parent) :
    QWidget(parent),
    m_startpage(nullptr),
    m_datasets(nullptr),
    m_cellview(nullptr),
    m_experiments(nullptr),
    m_stackWidget(nullptr),
    m_buttonGroup(nullptr),
    m_layout(nullptr),
    m_buttonLayout(nullptr),
    m_dataProxy(dataProxy)
{
    m_buttonGroup = new ExtendedButtonGroup();

    m_stackWidget = new QStackedWidget();
    m_stackWidget->setFrameShape(QFrame::StyledPanel);

    m_buttonLayout = new QVBoxLayout();
    m_buttonLayout->setSpacing(0);

    QVBoxLayout  *buttonStretchLayout = new QVBoxLayout();
    buttonStretchLayout->setSpacing(0);
    buttonStretchLayout->addLayout(m_buttonLayout);
    buttonStretchLayout->addStretch();

    m_layout = new QHBoxLayout();
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addLayout(buttonStretchLayout);
    m_layout->addWidget(m_stackWidget);
    setLayout(m_layout);

    createPages(); //create pages and add them to the tab manager
    createConnections(); //create signals - action connections

    // enter first pages
    tabChanged(0, -1);
}

ExtendedTabWidget::~ExtendedTabWidget()
{
    // exit last page
    const int index = currentIndex();
    tabChanged(-1, index);

    m_startpage->deleteLater();
    m_startpage = nullptr;

    m_datasets->deleteLater();
    m_datasets = nullptr;

    m_cellview->deleteLater();
    m_cellview = nullptr;

    m_experiments->deleteLater();
    m_experiments = nullptr;

    m_stackWidget->deleteLater();
    m_stackWidget = nullptr;

    m_buttonGroup->deleteLater();
    m_buttonGroup = nullptr;

    m_layout->deleteLater();
    m_layout = nullptr;

    m_buttonLayout->deleteLater();
    m_buttonLayout = nullptr;
}

QSize ExtendedTabWidget::sizeHint() const
{
    int xMax = 0;
    int yMax = 0;
    foreach(QAbstractButton  *button, m_buttonGroup->buttons()) {
        xMax = qMax(xMax, button->sizeHint().width());
        yMax = qMax(yMax, button->sizeHint().height());
    }
    return QSize(xMax, yMax);
}

int ExtendedTabWidget::count() const
{
    Q_ASSERT(!m_stackWidget.isNull());
    return m_stackWidget->count();
}

int ExtendedTabWidget::currentIndex() const
{
    Q_ASSERT(!m_stackWidget.isNull());
    return m_stackWidget->currentIndex();
}

void ExtendedTabWidget::insertPage(QWidget *page,
                                   const QIcon &icon, const QString &title)
{
    Q_ASSERT(!m_stackWidget.isNull());

    const int index = count();
    m_stackWidget->insertWidget(index, page);

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

    m_buttonGroup->addButton(button, index);
    m_buttonLayout->addWidget(button);
}

void ExtendedTabWidget::setCurrentIndex(int index)
{
    if (index < 0 || index >= count()) {
        index = 0;
    }

    if (index != currentIndex()) {
        m_stackWidget->setCurrentIndex(index);
        m_buttonGroup->button(index)->setChecked(true);
    }
}

QWidget* ExtendedTabWidget::widget(int index)
{
    return m_stackWidget->widget(index);
}

void ExtendedTabWidget::tabChanged(int toIndex, int fromIndex)
{
    qDebug() << QString("[PageContainerWidget] Page: %1 -> %2").arg(fromIndex).arg(toIndex);

    QWidget *fromWidget = widget(fromIndex);
    QWidget *toWidget = widget(toIndex);

    if (fromWidget != nullptr) {
        QMetaObject::invokeMethod(fromWidget, "onExit", Qt::DirectConnection);
    }

    if (toWidget != nullptr) {
        QMetaObject::invokeMethod(toWidget, "onEnter", Qt::DirectConnection);
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
    Q_ASSERT(!m_stackWidget.isNull());

    m_startpage = new InitPage(m_dataProxy, m_stackWidget);
    insertPage(m_startpage, QIcon(QStringLiteral(":/images/startpage-icon.png")), "Start");

    m_datasets = new DatasetPage(m_dataProxy, m_stackWidget);
    insertPage(m_datasets, QIcon(QStringLiteral(":/images/datasetpage-icon.png")), "Datasets");

    m_cellview = new CellViewPage(m_dataProxy, m_stackWidget);
    insertPage(m_cellview, QIcon(QStringLiteral(":/images/gene.png")), "Cell View");

    m_experiments = new ExperimentPage(m_dataProxy, m_stackWidget);
    insertPage(m_experiments, QIcon(QStringLiteral(":/images/experimentpage-icon.png")), "Analysis");
}

void ExtendedTabWidget::createConnections()
{
    // transition signals
    connect(m_startpage, SIGNAL(moveToNextPage()), this, SLOT(moveToNextPage()));
    connect(m_startpage, SIGNAL(moveToPreviousPage()), this, SLOT(moveToPreviousPage()));
    connect(m_datasets, SIGNAL(moveToNextPage()), this, SLOT(moveToNextPage()));
    connect(m_datasets, SIGNAL(moveToPreviousPage()), this, SLOT(moveToPreviousPage()));
    connect(m_cellview, SIGNAL(moveToNextPage()), this, SLOT(moveToNextPage()));
    connect(m_cellview, SIGNAL(moveToPreviousPage()), this, SLOT(moveToPreviousPage()));
    connect(m_experiments, SIGNAL(moveToNextPage()), this, SLOT(moveToNextPage()));
    connect(m_experiments, SIGNAL(moveToPreviousPage()), this, SLOT(moveToPreviousPage()));
}

void ExtendedTabWidget::resetStatus()
{
    qDebug() << "[ExtendedTabWidget] : reseting to start page..";
    for (int index = currentIndex(); index > 0; --index) {
        tabChanged(index - 1, index);
    }
}

void ExtendedTabWidget::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
}

void ExtendedTabWidget::KeyPressEvent(QKeyEvent *event)
{
    event->ignore();
}

void ExtendedTabWidget::wheelEvent(QWheelEvent *event)
{
    event->ignore();
}

bool ExtendedTabWidget::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::MouseButtonPress) {
        event->ignore();
        return true;
    }
    return QWidget::event(event);
}
