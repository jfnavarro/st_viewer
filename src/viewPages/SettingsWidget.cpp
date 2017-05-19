#include "SettingsWidget.h"
#include "ui_SettingsWidget.h"

#include <QWidgetAction>
#include <QComboBox>
#include <QSlider>
#include <QRadioButton>
#include <QGroupBox>
#include <QMenu>
#include <QColorDialog>
#include "customWidgets/SpinBoxSlider.h"

static const int GENE_INTENSITY_MIN = 1;
static const int GENE_INTENSITY_MAX = 10;
static const int GENE_SIZE_MIN = 5;
static const int GENE_SIZE_MAX = 30;

SettingsWidget::SettingsWidget(QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui::SettingsWidget)
{
    m_ui->setupUi(this);

    // TODO make connections (propagate signals)
}

SettingsWidget::~SettingsWidget()
{
}

void SettingsWidget::reset()
{
    //TODO disable signals

    //TODO ensable signals
}

void SettingsWidget::resetReadsThreshold(int min, int max)
{

}

void SettingsWidget::resetTotalReadsThreshold(int min, int max)
{

}

void SettingsWidget::resetTotalGenesThreshold(int min, int max)
{

}

void SettingsWidget::slotSetVisualMode(QAction *action)
{
    const QVariant variant = action->property("mode");
    if (variant.canConvert(QVariant::Int)) {
        const VisualMode mode = static_cast<VisualMode>(variant.toInt());
        emit signalVisualModeChanged(mode);
    } else {
        Q_ASSERT("Settings, Undefined gene visual mode!");
    }
}

void SettingsWidget::slotSetVisualTypeMode(QAction *action)
{
    const QVariant variant = action->property("mode");
    if (variant.canConvert(QVariant::Int)) {
        const VisualTypeMode mode = static_cast<VisualTypeMode>(variant.toInt());
        emit signalVisualTypeModeChanged(mode);
    } else {
        Q_ASSERT("Settings, Undefined gene visual type mode!");
    }
}

void SettingsWidget::slotSetNormalizationMode(QAction *action)
{
    const QVariant variant = action->property("mode");
    if (variant.canConvert(QVariant::Int)) {
        const NormalizationMode mode = static_cast<NormalizationMode>(variant.toInt());
        emit signalNormalizationModeChanged(mode);
    } else {
        Q_ASSERT("Settings, Undefined normalization mode!");
    }
}
