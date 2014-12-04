/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#ifndef PAGE_H
#define PAGE_H

#include <QWidget>
#include <QProgressDialog>
#include <QPointer>

class DataProxy;

// this an abstract class to represent the different pages
// for the tab manager
class Page : public QWidget
{
    Q_OBJECT

public:

    Page(QPointer<DataProxy> dataProxy, QWidget *parent = 0);
    virtual ~Page();

signals:

    //navigation signals
    void moveToNextPage();
    void moveToPreviousPage();

protected slots:

    virtual void onEnter() = 0;
    virtual void onExit() = 0;

private slots:

    //closes the progress bar and cancel current active downloads
    void slotCancelProgressBar();

private:

    QPointer<QProgressDialog> m_progressDialog;

protected:

    //launches a progress bar dialog (true = show, false = hide)
    void setWaiting(bool waiting = true, const QString &label = "Downloading data...");

    //helper functions to show messages
    void showInfo(const QString &header, const QString &body);
    void showWarning(const QString &header, const QString &body);
    void showError(const QString &header, const QString &body);

    QPointer<DataProxy> m_dataProxy;
};

#endif // PAGE_H
