/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef EXPERIMENTPAGE_H
#define EXPERIMENTPAGE_H

#include <QWidget>
#include <QModelIndex>

#include "Page.h"

#include "data/DataProxy.h"

class ExperimentsItemModel;
class QSortFilterProxyModel;

namespace Ui{
class Experiments;
} // namespace Ui //

class ExperimentPage : public Page
{
    Q_OBJECT

public:

    explicit ExperimentPage(QPointer<DataProxy> dataProxy, QWidget *parent = 0);
    virtual ~ExperimentPage();

public slots:

    void onEnter() override;
    void onExit() override;

private slots:

    void slotLoadSelections();
    void slotSelectionSelected(QModelIndex index);
    void slotExportSelections();
    void slotRemoveSelections();

private:

    QSortFilterProxyModel *selectionsProxyModel();
    ExperimentsItemModel *selectionsModel();

    Ui::Experiments *m_ui;
    QPointer<DataProxy> m_dataProxy;

    Q_DISABLE_COPY(ExperimentPage)
};

#endif  /* EXPERIMENTPAGE_H */

