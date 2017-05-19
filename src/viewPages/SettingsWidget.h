#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QWidget
{
    Q_OBJECT

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

public:
    explicit SettingsWidget(QWidget *parent = 0);
    ~SettingsWidget();

    void resetReadsThreshold(int min, int max);
    void resetTotalReadsThreshold(int min, int max);
    void resetTotalGenesThreshold(int min, int max);
    void reset();

public slots:

private slots:
    // select visual mode
    void slotSetVisualMode(QAction *action);
    // select visual type mode
    void slotSetVisualTypeMode(QAction *action);
    // select normalization mode
    void slotSetNormalizationMode(QAction *action);
signals:

    void signalNormalizationModeChanged(NormalizationMode);
    void signalVisualTypeModeChanged(VisualTypeMode);
    void signalVisualModeChanged(VisualMode);

private:
    QScopedPointer<Ui::SettingsWidget> m_ui;
};

#endif // SETTINGSWIDGET_H
