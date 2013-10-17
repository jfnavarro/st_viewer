/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef DATASETPAGE_H
#define	DATASETPAGE_H

#include <QWidget>

#include "controller/data/DataProxy.h"

class QItemSelectionModel;
class DatasetItemModel;
class QItemSelection;

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

public slots:

    void onInit();
    void onEnter();
    void onExit();

    void slotDataError(Error*);

    void datasetSelected(DataProxy::DatasetPtr);
    
    void refreshDatasets();
    
    void loadData();
    void dataLoaded();
    
private:
    
    Ui::DataSets *ui;
    DatasetItemModel *m_datasetModel;
    
    void setWaiting(bool waiting = true);

};

#endif	/* DATASETPAGE_H */

