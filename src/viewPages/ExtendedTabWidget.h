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

class ExtendedButtonGroup : public QButtonGroup
{
    //this class extends QButtonGroup to unable the mouse events
public:

    explicit ExtendedButtonGroup(QWidget* parent = 0) : QButtonGroup(parent){}

protected:

    virtual void mousePressEvent(QMouseEvent *event) { event->ignore(); }
    virtual void KeyPressEvent(QKeyEvent *event) { event->ignore(); }
    virtual void wheelEvent(QWheelEvent *event) { event->ignore(); }
    virtual bool event(QEvent *event);
};

class ExtendedButton : public QPushButton
{
    //this class extends QPushButton to unable the mouse events
public:

    explicit ExtendedButton(QWidget* parent = 0) : QPushButton(parent) {}
    explicit ExtendedButton(const QString &text, QWidget *parent = 0) : QPushButton(text, parent) {}
    explicit ExtendedButton(const QIcon& icon, const QString &text, QWidget *parent = 0): QPushButton(icon, text, parent) {}

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
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex STORED true NOTIFY currentIndexChanged)

public:

    explicit ExtendedTabWidget(QWidget *parent = 0);
    virtual ~ ExtendedTabWidget();

    QSize sizeHint() const;
    int count() const;
    int currentIndex() const;
    QWidget *widget(int index);
    int indexOf(QWidget* w) const;
    void addTab(QWidget *page, const QString &title);
    void addTab(QWidget *page, const QIcon &icon = QIcon(), const QString &title = QString());

public slots:

    void addPage(QWidget *page, const QIcon &icon = QIcon(), const QString &title = QString());
    void insertPage(int index, QWidget *page, const QIcon &icon = QIcon(), const QString &title = QString());
    void removePage(int index);
    void setCurrentIndex(int index);
    void tabChanged(int toIndex, int fromIndex = -1);
    void moveToNextPage();
    void moveToPreviousPage();
    void buttonClicked(int);
    //it sets the start page as current page going trough all the previuos pages (needed when we clean cache)
    void resetStatus();

signals:

    void currentIndexChanged(int index);
    void signalError(Error*);

protected:

    virtual void mousePressEvent(QMouseEvent *event) { event->ignore(); }
    virtual void KeyPressEvent(QKeyEvent *event) { event->ignore(); }
    virtual void wheelEvent(QWheelEvent *event) { event->ignore(); }
    virtual bool event(QEvent *event);

private:

    void createPages();
    void createActions();

    //pages
    InitPage *startpage;
    DatasetPage *datasets;
    CellViewPage *cellview;
    ExperimentPage *experiments;

    //components
    QStackedWidget *stackWidget;
    QButtonGroup *buttonGroup;
    QHBoxLayout *layout;
    QVBoxLayout *buttonLayout;
};
#endif  /* // EXTENDEDTABWIDGET_H */
