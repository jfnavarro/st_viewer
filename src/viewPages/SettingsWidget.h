#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:

    enum VisualTypeMode {
        Reads = 1,
        ReadsLog = 2,
        Genes = 3,
        GenesLog = 4
    };

    enum NormalizationMode {
        RAW = 1,
        TPM = 2,
        REL = 3,
        DESEQ = 4,
        SCRAN = 5
    };

    enum VisualMode {
        Normal = 1,
        DynamicRange = 2,
        HeatMap = 3,
        ColorRange = 4
    };

    struct Rendering {
        int reads_threshold;
        int genes_threshold;
        int spots_threshold;
        int ind_reads_threshold;
        double legend_min;
        double legend_max;
        double intensity;
        double size;
        VisualMode visual_mode;
        NormalizationMode normalization_mode;
        VisualTypeMode visual_type_mode;
        bool gene_cutoff;
        bool size_factors;
    };

    explicit SettingsWidget(QWidget *parent = nullptr);
    ~SettingsWidget();

    void reset();
    Rendering &renderingSettings();

public slots:

private slots:

    void slotGenesTreshold(int);
    void slotSpotsTreshold(int);
    void slotReadsTreshold(int);
    void slotIndReadsTreshold(int);
    void slotIntensity(int);
    void slotSize(int);
    void slotGeneCutoff(bool);
    void slotSizeFactors(bool);
    void slotNormalization(NormalizationMode);
    void slotVisualMode(VisualMode);
    void slotVisualMode(VisualTypeMode);

signals:

    void signalShowSpots(bool);
    void signalShowLegend(bool);
    void signalShowImage(bool);
    void signalSpotRendering();

private:
    QScopedPointer<Ui::SettingsWidget> m_ui;
    Rendering m_rendering_settings;
};

#endif // SETTINGSWIDGET_H
