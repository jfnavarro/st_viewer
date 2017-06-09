#include "SelectionSpotsWidget.h"

#include <QSortFilterProxyModel>
#include "SettingsStyle.h"

#include "ui_spotsSelectionWidget.h"

using namespace Style;

SelectionSpotsWidget::SelectionSpotsWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_ui(new Ui::spotsSelectionWidget())
{
    m_ui->setupUi(this);
}

SelectionSpotsWidget::~SelectionSpotsWidget()
{
}

void SelectionSpotsWidget::loaData(const UserSelection::SpotListType &spots,
                                   const UserSelection::Matrix &counts)
{
    Q_UNUSED(spots)
    Q_UNUSED(counts)
}

