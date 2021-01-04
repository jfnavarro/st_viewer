#include "ClustersWidget.h"

#include <QMenu>
#include <QVBoxLayout>
#include <QWidgetAction>
#include <QPushButton>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QAction>
#include <QColorDialog>
#include <QMessageBox>

#include "viewTables/ClusterTableView.h"
#include "model/ClusterItemModel.h"
#include "data/Cluster.h"

#include "SettingsStyle.h"

using namespace Style;

ClustersWidget::ClustersWidget(QWidget *parent)
    : QWidget(parent)
    , m_clusters_tableview(nullptr)
    , m_colorList(nullptr)
{
    // one layout for the controls and another for the table
    QVBoxLayout *clustersLayout = new QVBoxLayout();
    clustersLayout->setSpacing(0);
    clustersLayout->setContentsMargins(10, 10, 10, 10);
    QHBoxLayout *clustersListLayout = new QHBoxLayout();
    clustersListLayout->setSpacing(0);
    clustersListLayout->setContentsMargins(0, 5, 0, 5);

    // add separation between buttons
    clustersListLayout->addSpacing(10);

    QPushButton *showSelectedButton = new QPushButton(this);
    configureButton(showSelectedButton,
                    QIcon(QStringLiteral(":/images/visible.png")),
                    tr("Show selected clusters"));
    clustersListLayout->addWidget(showSelectedButton);
    // add separation
    clustersListLayout->addSpacing(CELL_PAGE_SUB_MENU_BUTTON_SPACE);

    QPushButton *hideSelectedButton = new QPushButton(this);
    configureButton(hideSelectedButton,
                    QIcon(QStringLiteral(":/images/nonvisible.png")),
                    tr("Hide selected clusters"));
    clustersListLayout->addWidget(hideSelectedButton);
    // add separation
    clustersListLayout->addSpacing(CELL_PAGE_SUB_MENU_BUTTON_SPACE);


    QPushButton *showColorButton = new QPushButton(this);
    configureButton(showColorButton,
                    QIcon(QStringLiteral(":/images/select-color.png")),
                    tr("Set color of selected clusters"));
    // show color button will open up a color selector
    m_colorList.reset(new QColorDialog(Qt::white, this));
    m_colorList->setOption(QColorDialog::DontUseNativeDialog, true);
    clustersListLayout->addWidget(showColorButton);
    // add separation
    clustersListLayout->addSpacing(CELL_PAGE_SUB_MENU_BUTTON_SPACE);

    // add actions menu to main layout
    clustersLayout->addLayout(clustersListLayout);

    // create spots table
    m_clusters_tableview.reset(new ClusterTableView(this));
    // add table to main layout
    clustersLayout->addWidget(m_clusters_tableview.data());

    // set main layout
    setLayout(clustersLayout);

    // connections
    connect(showSelectedButton, &QPushButton::clicked, [=]() { slotSetVisible(true); });
    connect(hideSelectedButton, &QPushButton::clicked, [=]() { slotSetVisible(false); });
    connect(showColorButton, &QPushButton::clicked, [=] {
        m_colorList->show();
        m_colorList->raise();
        m_colorList->activateWindow();
    });
    connect(m_colorList.data(), &QColorDialog::colorSelected, [=]() {
        slotSetColor(m_colorList->currentColor());
    });
    connect(m_clusters_tableview.data(),
            &ClusterTableView::signalUpdated,
            this,
            &ClustersWidget::signalUpdated);
}

ClustersWidget::~ClustersWidget()
{

}

void ClustersWidget::clear()
{
    m_clusters_tableview->clearSelection();
    m_clusters_tableview->clearFocus();
    m_clusters_tableview->getModel()->clear();
    m_colorList->setCurrentColor(Qt::red);
}

void ClustersWidget::updateModelTable()
{
    m_clusters_tableview->update();
}

void ClustersWidget::configureButton(QPushButton *button, const QIcon &icon, const QString &tooltip)
{
    Q_ASSERT(button != nullptr);
    button->setIcon(icon);
    button->setIconSize(CELL_PAGE_SUB_MENU_ICON_SIZE);
    button->setFixedSize(CELL_PAGE_SUB_MENU_BUTTON_SIZE);
    button->setStyleSheet(CELL_PAGE_SUB_MENU_BUTTON_STYLE);
    button->setCursor(Qt::PointingHandCursor);
    button->setToolTip(tooltip);
    button->setStatusTip(tooltip);
}

void ClustersWidget::slotSetVisible(bool visible)
{
    m_clusters_tableview->getModel()->setVisible(m_clusters_tableview->getItemSelection(), visible);
    m_clusters_tableview->update();
    emit signalUpdated();
}

void ClustersWidget::slotSetColor(const QColor &color)
{
    m_clusters_tableview->getModel()->setColor(m_clusters_tableview->getItemSelection(), color);
    m_clusters_tableview->update();
    emit signalUpdated();
}

void ClustersWidget::slotLoadClusters(const STData::ClusterListType &clusters)
{
    m_clusters_tableview->getModel()->loadData(clusters);
    m_clusters_tableview->update();
}
