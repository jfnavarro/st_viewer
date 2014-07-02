/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef EXTENDEDTABWIDGET_H
#define EXTENDEDTABWIDGET_H

#include <QWidget>
#include <QButtonGroup>
#include <QPushButton>
#include <QMouseEvent>
#include <QPointer>

class InitPage;
class DatasetPage;
class CellViewPage;
class ExperimentPage;
class QIcon;
class QComboBox;
class QStackedWidget;
class QVBoxLayout;
class QHBoxLayout;
class Error;

//this class extends QButtonGroup to unable the mouse events
class ExtendedButtonGroup : public QButtonGroup
{

public:

    explicit ExtendedButtonGroup(QWidget* parent = 0) : QButtonGroup(parent){}

protected:

    virtual void mousePressEvent(QMouseEvent *event) { event->ignore(); }
    virtual void KeyPressEvent(QKeyEvent *event) { event->ignore(); }
    virtual void wheelEvent(QWheelEvent *event) { event->ignore(); }
    virtual bool event(QEvent *event);
};

 //this class extends QPushButton to unable the mouse events
class ExtendedButton : public QPushButton
{

public:

    explicit ExtendedButton(QWidget* parent = 0) :
        QPushButton(parent) {}
    ExtendedButton(const QString &text, QWidget *parent = 0) :
        QPushButton(text, parent) {}
    ExtendedButton(const QIcon& icon, const QString &text, QWidget *parent = 0):
        QPushButton(icon, text, parent) {}

protected:

    virtual void mousePressEvent(QMouseEvent *event) { event->ignore(); }
    virtual void KeyPressEvent(QKeyEvent *event) { event->ignore(); }
    virtual void wheelEvent(QWheelEvent *event) { event->ignore(); }
    virtual bool event(QEvent *event);

};

//customized tab manager based on a QStackedWidget and a QButtonGroup.
//it has the same functionality as the QTabWidget.
//Pages are member variables
class ExtendedTabWidget : public QWidget
{
    Q_OBJECT

public:

    explicit ExtendedTabWidget(QWidget *parent = 0);
    virtual ~ ExtendedTabWidget();

    QSize sizeHint() const;
    int count() const;
    int currentIndex() const;

public slots:

    void moveToNextPage();
    void moveToPreviousPage();
    //it sets the start page as current page going trough all
    //the previuos pages (needed when we clean cache)
    void resetStatus();

signals:

    void currentIndexChanged(int index);

protected:

    virtual void mousePressEvent(QMouseEvent *event) { event->ignore(); }
    virtual void KeyPressEvent(QKeyEvent *event) { event->ignore(); }
    virtual void wheelEvent(QWheelEvent *event) { event->ignore(); }
    virtual bool event(QEvent *event);

private:

    QWidget* widget(int index);

    void insertPage(QWidget* page, const QIcon &icon = QIcon(),
                    const QString &title = QString());

    void setCurrentIndex(int index);

    void tabChanged(int toIndex, int fromIndex = -1);

    void createPages();
    void createActions();

    //pages
    QPointer<InitPage> startpage;
    QPointer<DatasetPage> datasets;
    QPointer<CellViewPage> cellview;
    QPointer<ExperimentPage> experiments;

    //components
    QPointer<QStackedWidget> stackWidget;
    QPointer<QButtonGroup> buttonGroup;
    QPointer<QHBoxLayout> layout;
    QPointer<QVBoxLayout> buttonLayout;

    Q_DISABLE_COPY(ExtendedTabWidget)
};
#endif  /* // EXTENDEDTABWIDGET_H */
