#include "DataRenderer.h"

/*DataRenderer::DataRenderer()
{

}

DataRenderer::run()
{

    // temp local variables to store the genes/reads/tpm/color of each feature
    QColor indexColor = Globals::DEFAULT_COLOR_GENE;
    int indexValue = 0;
    int indexValueGenes = 0;

    // iterate the features to compute rendering data for an specific index (position)
    GeneRendererGL::GeneInfoByIndexMap::const_iterator it = m_geneInfoByIndex.constFind(index);
    GeneRendererGL::GeneInfoByIndexMap::const_iterator end = m_geneInfoByIndex.constEnd();
    for (; it != end && it.key() == index; ++it) {
        DataProxy::FeaturePtr feature = it.value();
        Q_ASSERT(feature);

        const int currentHits = feature->hits();
        // check if the reads are outside the threshold
        if (featureReadsOutsideRange(currentHits)) {
            continue;
        }

        // if we want to enforce the selection we add the feature to the container
        if (m_forceSelection) {
            m_geneInfoSelectedFeatures.append(feature);
        }

        // we check if gene is selected here because we want to select all the genes
        // in the feature when we are forcing selection
        const auto gene = feature->geneObject();
        if (!gene->selected()) {
            continue;
        }

        // update local variables for number of reads and genes
        indexValue += currentHits;
        ++indexValueGenes;

        // when the color of the new feature is different than the color
        // in the feature's index we do linear interpolation adjusted
        // by the number of genes in the feature to obtain the new color
        const QColor &featureColor = gene->color();
        if (indexColor != featureColor) {
            const qreal adjustment = 1.0 / indexValueGenes;
            indexColor = STMath::lerp(adjustment, indexColor, featureColor);
        }
    }

    // we only show features where there is at least one gene activated
    const bool visible = indexValueGenes != 0;

    // update pooled min-max to compute colors if applies
    if (isPooled && visible) {
        if (pooling_genes) {
            indexValue = indexValueGenes;
        } else if (pooling_tpm) {
            indexValue = STMath::tpmNormalization<int>(indexValue, total_reads_feature);
        }
        // only update the boundaries for color computation in pooled mode
        m_localPooledMin = std::min(indexValue, m_localPooledMin);
        m_localPooledMax = std::max(indexValue, m_localPooledMax);
    }

    // update rendering data arrays
    m_geneData.updateQuadReads(index, indexValue);
    m_geneData.updateQuadSelected(index, visible && forceSelection);
    m_geneData.updateQuadVisible(index, visible);
    m_geneData.updateQuadColor(index, indexColor);
}
*/
