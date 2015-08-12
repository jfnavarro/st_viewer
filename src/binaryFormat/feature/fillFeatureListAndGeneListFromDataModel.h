#ifndef FILL_FEATURELIST_AND_GENELIST_FROM_DATAMODEL_H
#define FILL_FEATURELIST_AND_GENELIST_FROM_DATAMODEL_H

#include <data/DataProxy.h>
#include <binaryFormat/feature/datamodel.h>

void fillFeaturelistAndGenelistFromDatamodel(const data_model::Everything &everything, DataProxy::FeatureList *featureList, DataProxy::GeneList *geneList);

#endif
