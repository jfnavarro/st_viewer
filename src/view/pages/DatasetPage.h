/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef DATASETPAGE_H
#define	DATASETPAGE_H

#include <QWidget>

class DatasetItemModel;
namespace Ui
{
    class DataSets;
} // namespace Ui //

// this is the definition of the datasets page which contains a list of datasets to be selected
// it gets updated everytime we enter the page and by selecting a dataset we invoke the dataproxy to load the data
// as every page it implements the moveToNextPage and moveToPreviousPage
// the methods onEnter and onExit are called dynamically from the page manager.

class Error;

class DatasetPage : public QWidget
{
    Q_OBJECT
    
public:
    
    explicit DatasetPage(QWidget *parent = 0);
    virtual ~DatasetPage();

signals:
    
    void signalError(Error* error);

    void moveToNextPage();
    void moveToPreviousPage();

    void datasetSelected(const QString& datasetId);

public slots:

    void onInit();
    void onEnter();
    void onExit();

    void slotDataError(Error*);
    
    void datasetSelected(QModelIndex index);
    
    void refreshDatasets();
    
    void loadData();
    void dataLoaded();
    
protected:

    DatasetItemModel *datasetModel;

private:
    
    Ui::DataSets *ui;

    //TODO move to abstract super class!
    void setWaiting(bool waiting = true);

    /* selected dataset */
    QString m_datasetId;
};

#endif	/* DATASETPAGE_H */

