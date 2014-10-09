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
class AuthorizationManager;
class DataProxy;

//this class extends QButtonGroup to unable the mouse events
//TODO move to a separate class
class ExtendedButtonGroup : public QButtonGroup
{

public:

    explicit ExtendedButtonGroup(QWidget* parent = 0);

protected:
    //we do not want the user to interact with the buttons
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void KeyPressEvent(QKeyEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    bool event(QEvent *event) override;
};

 //this class extends QPushButton to unable the mouse events
//TODO move to a separate class
class ExtendedButton : public QPushButton
{

public:

    explicit ExtendedButton(QWidget* parent = 0);
    ExtendedButton(const QString &text, QWidget *parent = 0);
    ExtendedButton(const QIcon& icon, const QString &text, QWidget *parent = 0);

protected:
    //we do not want the user to interact with the buttons
    void mousePressEvent(QMouseEvent *event) override;
    virtual void KeyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event) override;
    bool event(QEvent *event) override;

};

//customized tab manager based on a QStackedWidget and a QButtonGroup.
//it has the same functionality as the QTabWidget.
//Pages are member variables
class ExtendedTabWidget : public QWidget
{
    Q_OBJECT

public:

    explicit ExtendedTabWidget(QPointer<DataProxy> dataProxy,
                               QPointer<AuthorizationManager> authManager,
                               QWidget *parent = 0);
    virtual ~ExtendedTabWidget();

    QSize sizeHint() const;
    int count() const;
    int currentIndex() const;

public slots:

    //TODO slots should have the previx "slot"

    void moveToNextPage();
    void moveToPreviousPage();

    //sets the start page as current page going trough all
    //the previuos pages (needed when we clean cache)
    void resetStatus();

protected:
    //we do not want the user to interact with the buttons
    void mousePressEvent(QMouseEvent *event) override;
    virtual void KeyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event) override;
    bool event(QEvent *event) override;

private:

    //returns the widget(page) for a given index
    QWidget* widget(int index);

    //adds page to the tab manager
    void insertPage(QWidget* page, const QIcon &icon = QIcon(),
                    const QString &title = QString());

    //set the current index in the tab manager
    void setCurrentIndex(int index);

    //makes a page transition
    void tabChanged(int toIndex, int fromIndex = -1);

    //helper function to instanciate the pages and create the connections
    void createPages();
    void createConnections();

    //pages
    QPointer<InitPage> m_startpage;
    QPointer<DatasetPage> m_datasets;
    QPointer<CellViewPage> m_cellview;
    QPointer<ExperimentPage> m_experiments;

    //UI components
    QPointer<QStackedWidget> m_stackWidget;
    QPointer<QButtonGroup> m_buttonGroup;
    QPointer<QHBoxLayout> m_layout;
    QPointer<QVBoxLayout> m_buttonLayout;

    //refence to dataProxy and auth manager
    QPointer<DataProxy> m_dataProxy;
    QPointer<AuthorizationManager> m_authManager;

    Q_DISABLE_COPY(ExtendedTabWidget);
};
#endif  /* // EXTENDEDTABWIDGET_H */
