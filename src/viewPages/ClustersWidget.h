#ifndef CLUSTERSWDIGET_H
#define CLUSTERSWDIGET_H

#include <QDockWidget>
#include <QIcon>

#include "data/STData.h"

class QPushButton;
class ClusterTableView;
class QColorDialog;

// This widget is componsed of the clusters table
// a search field and the select and action menus so the user can
// select/deselect clusters and change their color.
// Every action will affect what spots are shown in the cell view.
class ClustersWidget : public QWidget
{
    Q_OBJECT

public:

    explicit ClustersWidget(QWidget *parent = nullptr);
    virtual ~ClustersWidget() override;

    // clear variables
    void clear();

    // forces an update of the table
    void updateModelTable();

signals:

    // signals emitted when the user selects or change colors
    void signalUpdated();

public slots:

    // the user has loaded or created clusters
    void slotLoadClusters(const STData::ClusterListType &clusters);

private slots:

    // slots triggered by the show/color/selected controls in the clusters table
    void slotSetColor(const QColor &color);
    void slotSetVisible(bool visible);

private:
    // internal function to configure created buttons
    // to avoid code duplication
    // TODO better approach would be to have factories somewhere else
    void configureButton(QPushButton *button, const QIcon &icon, const QString &tooltip);

    // some references needed to UI elements
    QScopedPointer<ClusterTableView> m_clusters_tableview;
    QScopedPointer<QColorDialog> m_colorList;

    Q_DISABLE_COPY(ClustersWidget)

};

#endif // CLUSTERSWDIGET_H
