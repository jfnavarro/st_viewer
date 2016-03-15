/*
#ifndef DATARENDERER_H
#define DATARENDERER_H


#include <QRunnable>
#include "viewOpenGL/GeneRendererGL.h"

class GeneData;

class DataRenderer : public QRunnable
{

    DataRenderer(const int total_genes_feature,
                 const int total_reads_feature,
                 const bool forceSelection,
                 GeneRendererGL::GeneInfoByIndexMap &geneInfoByIndex,
                 GeneRendererGL::GeneInfoSelectedFeatures &geneInfoSelectedFeatures,
                 int &localPooledMin,
                 int &localPooledMax,
                 GeneData &geneData,
                 void (*featureReadsOusideRange)(int,int)
                 );

private:

    void run();

    const int m_total_genes_feature;
    const int m_total_reads_feature;
    const bool m_forceSelection;
    GeneRendererGL::GeneInfoByIndexMap &m_geneInfoByIndex;
    GeneRendererGL::GeneInfoSelectedFeatures &m_geneInfoSelectedFeatures;
    int &m_localPooledMin;
    int &m_localPooledMax;
    GeneData &m_geneData;
    void (*featureReadsOusideRange)(int,int);
};
*/
