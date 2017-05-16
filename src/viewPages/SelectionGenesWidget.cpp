#include "SelectionGenesWidget.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QSortFilterProxyModel>

#include "utils/SetTips.h"
#include "viewTables/GenesTableView.h"
#include "SettingsStyle.h"

using namespace Style;

SelectionGenesWidget::SelectionGenesWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{

}

SelectionGenesWidget::~SelectionGenesWidget()
{
}

void SelectionGenesWidget::clear()
{

}

void SelectionGenesWidget::slotLoadModel(const UserSelection &selection)
{
    Q_UNUSED(selection)
}

