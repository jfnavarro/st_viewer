#include "utils/SetTips.h"

#include <QString>
#include <QWidget>
#include <QAction>

void setToolTipAndStatusTip(const QString& str, QWidget* widget)
{
    Q_ASSERT(widget);
    widget->setToolTip(str);
    widget->setStatusTip(str);
}

void setToolTipAndStatusTip(const QString& str, QAction* action)
{
    Q_ASSERT(action);
    action->setToolTip(str);
    action->setStatusTip(str);
}
