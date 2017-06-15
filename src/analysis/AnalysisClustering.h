#ifndef ANALYSISCLUSTERING_H
#define ANALYSISCLUSTERING_H

#include <QDialog>
#include "data/STData.h"

namespace Ui {
class analysisClustering;
}

// A Widget used to classify the spots based on gene expression profiles
// using t-SNE and KMeans.
class AnalysisClustering : public QWidget
{
    Q_OBJECT

public:
    explicit AnalysisClustering(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~AnalysisClustering();

    // One color for each spot
    QVector<QColor> getComputedClasses() const;

    // assigns the dataset
    void loadData(const STData::STDataFrame &data);

public slots:

signals:

    void singalClusteringUpdated();

private slots:

    // Performs a dimensionality reduction (t-SNE) on the data matrix and then
    // cluster the reduced coordinates (2D) using KMeans so to compute classes/colors
    // for each spot
    void run();

private:

    STData::STDataFrame m_data;
    QVector<QColor> m_computed_colors;

    // store the size factors to save computational time
    rowvec m_deseq_factors;
    rowvec m_scran_factors;

    QScopedPointer<Ui::analysisClustering> m_ui;
};

#endif // ANALYSISCLUSTERING_H
