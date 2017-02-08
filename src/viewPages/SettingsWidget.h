#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class SettingsWidget;
}

class QSlider;
class SpinBoxSlider;
class QComboBox;
class QRadioButton;

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = 0);
    ~SettingsWidget();

public slots:
    // some slots for gene actions that need adjustment of the value (scale)
    void slotSpotShape(int geneShape);
    void slotSpotIntensity(int geneIntensity);
    void slotSpotSize(int geneSize);
    // select gene visual mode
    void slotSetGeneVisualMode(QAction *action);
    // select legend anchor
    void slotSetLegendAnchor(QAction *action);

private:
    Ui::SettingsWidget *ui;

    // Elements of the cell view visual settings menu
    QScopedPointer<QRadioButton> m_colorLinear;
    QScopedPointer<QRadioButton> m_colorLog;
    QScopedPointer<QRadioButton> m_colorExp;
    QScopedPointer<QRadioButton> m_poolingGenes;
    QScopedPointer<QRadioButton> m_poolingReads;
    QScopedPointer<QRadioButton> m_poolingTPMs;
    QScopedPointer<SpinBoxSlider> m_geneHitsThreshold;
    QScopedPointer<SpinBoxSlider> m_geneGenesThreshold;
    QScopedPointer<SpinBoxSlider> m_geneTotalReadsThreshold;
    QScopedPointer<QSlider> m_geneIntensitySlider;
    QScopedPointer<QSlider> m_geneSizeSlider;
    QScopedPointer<QComboBox> m_geneShapeComboBox;

};

#endif // SETTINGSWIDGET_H
