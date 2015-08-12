#include <binaryFormat/feature/fillFeatureListAndGeneListFromDataModel.h>

#include <QDebug>

void fillFeaturelistAndGenelistFromDatamodel(const data_model::Everything &data_model_everything, DataProxy::FeatureList *featureList, DataProxy::GeneList *geneList) {
  qDebug() << "aaaaaaa in fillFeaturelistAndGenelistFromDatamodel";

  std::vector< DataProxy::GenePtr * > tmp_vector;
  
  for (const auto &gene_name : data_model_everything.gene_names) {
    geneList->push_back(DataProxy::GenePtr(new Gene(QString::fromStdString(gene_name))));
    tmp_vector.push_back(&(geneList->back()));
  }
  int gene_index = 0;
  for (const auto &gene : data_model_everything.genes) {
    int y_index = 0;
    for (const auto &horizontal_line : gene.horizontal_lines) {
      for (const auto &position_hit : horizontal_line.position_hits) {
        const int x = position_hit.x;
	const auto y = y_index;
	DataProxy::FeaturePtr feature = DataProxy::FeaturePtr(new Feature(QString::fromStdString(data_model_everything.barcode_map.at( data_model::two_dimensional_coord_t(x, y))), QString::fromStdString(data_model_everything.gene_names[gene_index]), x, y, position_hit.hits ));

        feature->geneObject((*(tmp_vector[gene_index])));
        featureList->push_back(feature);
      }
      ++y_index;
    }
    ++gene_index;
  }
}  
  
