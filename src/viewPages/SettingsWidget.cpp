#include "SettingsWidget.h"
#include "ui_SettingsWidget.h"
#include <QDebug>

constexpr int INTENSITY_MIN = 1;
constexpr int INTENSITY_MAX = 10;
constexpr int SIZEMIN = 1;
constexpr int SIZEMAX = 25;
constexpr int SIZE_DEFAULT = 10;
constexpr double INTENSITY_DEFAULT = 1.0;

SettingsWidget::SettingsWidget(QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui::SettingsWidget)
{
    m_ui->setupUi(this);
    setWindowFlag(Qt::WindowStaysOnTopHint, true);

    // Reset elemements to default
    reset();

    // Make connections (propagate signals)
    connect(m_ui->genes_threshold,
            static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &SettingsWidget::slotGenesTreshold);
    connect(m_ui->reads_threshold,
            static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &SettingsWidget::slotReadsTreshold);
    connect(m_ui->spots_threshold,
            static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &SettingsWidget::slotSpotsTreshold);

    connect(m_ui->spots_intensity,
            &QSlider::valueChanged, this, &SettingsWidget::slotIntensity);
    connect(m_ui->spots_size,
            &QSlider::valueChanged, this, &SettingsWidget::slotSize);

    connect(m_ui->show_image, &QCheckBox::stateChanged, this, &SettingsWidget::signalShowImage);
    connect(m_ui->show_spots, &QCheckBox::stateChanged, this, [=] {
        m_rendering_settings.show_spots = m_ui->show_spots->isChecked();
    });
    connect(m_ui->legend, &QCheckBox::stateChanged, this, &SettingsWidget::signalShowLegend);

    connect(m_ui->normalization_raw, &QRadioButton::clicked, this,
            [=]() {slotNormalization(NormalizationMode::RAW);});
    connect(m_ui->normalization_tpm, &QRadioButton::clicked, this,
            [=]() {slotNormalization(NormalizationMode::CPM);});
    connect(m_ui->normalization_rel, &QRadioButton::clicked, this,
            [=]() {slotNormalization(NormalizationMode::REL);});
    connect(m_ui->log_scale, &QCheckBox::stateChanged, this, [=] {
        m_rendering_settings.log_scale = m_ui->log_scale->isChecked();
    });
    connect(m_ui->zcore, &QCheckBox::stateChanged, this, [=] {
        m_rendering_settings.zscore = m_ui->zcore->isChecked();
    });

    connect(m_ui->visual_normal, &QRadioButton::clicked, this,
            [=]() {slotVisualMode(Normal);});
    connect(m_ui->visual_dynamic_range, &QRadioButton::clicked, this,
            [=]() {slotVisualMode(DynamicRange);});
    connect(m_ui->visual_heatmap, &QRadioButton::clicked, this,
            [=]() {slotVisualMode(HeatMap);});
    connect(m_ui->visual_color_range, &QRadioButton::clicked, this,
            [=]() {slotVisualMode(ColorRange);});


    connect(m_ui->update, &QPushButton::clicked, this, &SettingsWidget::signalSpotRendering);
}

SettingsWidget::~SettingsWidget()
{
}

void SettingsWidget::reset()
{
    const QSignalBlocker blocker(this);
    m_ui->spots_intensity->setMinimum(INTENSITY_MIN);
    m_ui->spots_intensity->setMaximum(INTENSITY_MAX);
    m_ui->spots_intensity->setValue(INTENSITY_MAX);
    m_ui->spots_size->setMinimum(SIZEMIN);
    m_ui->spots_size->setMaximum(SIZEMAX);
    m_ui->spots_size->setValue(SIZE_DEFAULT);
    m_ui->show_image->setChecked(true);
    m_ui->show_spots->setChecked(false);
    m_ui->log_scale->setChecked(false);
    m_ui->legend->setChecked(false);
    m_ui->normalization_raw->setChecked(true);
    m_ui->visual_normal->setChecked(true);
    m_ui->reads_threshold->setMinimum(0);
    m_ui->reads_threshold->setValue(0);
    m_ui->reads_threshold->setMaximum(1000);
    m_ui->genes_threshold->setMinimum(0);
    m_ui->genes_threshold->setValue(0);
    m_ui->genes_threshold->setMaximum(20000);
    m_ui->spots_threshold->setMinimum(0);
    m_ui->spots_threshold->setValue(0);
    m_ui->spots_threshold->setMaximum(20000);
    m_rendering_settings.intensity = INTENSITY_DEFAULT;
    m_rendering_settings.size = SIZE_DEFAULT;
    m_rendering_settings.gene_cutoff = false;
    m_rendering_settings.genes_threshold = 0;
    m_rendering_settings.spots_threshold = 0;
    m_rendering_settings.reads_threshold = 0;
    m_rendering_settings.legend_max = 1;
    m_rendering_settings.legend_min = 0;
    m_rendering_settings.normalization_mode = SettingsWidget::RAW;
    m_rendering_settings.visual_mode = SettingsWidget::Normal;
    m_rendering_settings.show_spots = false;
    m_rendering_settings.log_scale = false;
    m_rendering_settings.zscore = false;
}

SettingsWidget::Rendering &SettingsWidget::renderingSettings()
{
    return m_rendering_settings;
}

void SettingsWidget::slotGenesTreshold(int value)
{
    if (value != m_rendering_settings.genes_threshold) {
        m_rendering_settings.genes_threshold = value;
    }
}

void SettingsWidget::slotSpotsTreshold(int value)
{
    if (value != m_rendering_settings.spots_threshold) {
        m_rendering_settings.spots_threshold = value;
    }
}

void SettingsWidget::slotReadsTreshold(int value)
{
    if (value != m_rendering_settings.reads_threshold) {
        m_rendering_settings.reads_threshold = value;
    }
}

void SettingsWidget::slotIntensity(int value)
{
    const double intensity = static_cast<double>(value) / 10;
    if (intensity != m_rendering_settings.intensity) {
        m_rendering_settings.intensity = intensity;
        emit signalRendering();
    }
}

void SettingsWidget::slotSize(int value)
{
    if (value != m_rendering_settings.size) {
        m_rendering_settings.size = value;
        emit signalRendering();
    }
}

void SettingsWidget::slotGeneCutoff(bool value)
{
    if (value != m_rendering_settings.gene_cutoff) {
        m_rendering_settings.gene_cutoff = value;
    }
}

void SettingsWidget::slotNormalization(NormalizationMode mode)
{
    if (mode != m_rendering_settings.normalization_mode) {
        m_rendering_settings.normalization_mode = mode;
    }
}

void SettingsWidget::slotVisualMode(VisualMode mode)
{
    if (mode != m_rendering_settings.visual_mode) {
        m_rendering_settings.visual_mode = mode;
    }
}
