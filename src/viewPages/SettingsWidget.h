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
        int reads_min_threshold;
        int reads_max_threshold;
        int genes_threshold;
        int genes_min_threshold;
        int genes_max_threshold;
        int ind_reads_threshold;
        int ind_reads_min_threshold;
        int ind_reads_max_threshold;
        float intensity;
        float size;
        VisualMode visual_mode;
        NormalizationMode normalization_mode;
        VisualTypeMode visual_type_mode;
        bool gene_cutoff;
    };

    explicit SettingsWidget(QWidget *parent = 0);
    ~SettingsWidget();

    void resetReadsThreshold(int min, int max);
    void resetTotalReadsThreshold(int min, int max);
    void resetTotalGenesThreshold(int min, int max);
    void reset();
    const Rendering &renderingSettings() const;

public slots:

private slots:

    void slotGenesTreshold(int);
    void slotReadsTreshold(int);
    void slotIndReadsTreshold(int);
    void slotIntensity(int);
    void slotSize(int);
    void slotGeneCutoff(bool);
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
