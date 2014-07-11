/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef PAGE_H
#define PAGE_H

#include <QWidget>
#include <QProgressDialog>
#include <QTimer>
#include <QPointer>

// this an abstract class to represent the different pages
// for the tab manager
class Page : public QWidget
{
    Q_OBJECT

public:

    explicit Page(QWidget *parent = 0);
    virtual ~Page();

signals:

    void moveToNextPage();
    void moveToPreviousPage();

public slots:

    virtual void onEnter() = 0;
    virtual void onExit() = 0;

private slots:

    void increaseBar();

protected:

    void setWaiting(bool waiting = true);

    void showInfo(const QString &header, const QString &body);
    void showWarning(const QString &header, const QString &body);
    void showError(const QString &header, const QString &body);

private:

    QPointer<QProgressDialog> m_progressDialog;
    QPointer<QTimer> m_timer;
    int m_steps;
};

#endif // PAGE_H
