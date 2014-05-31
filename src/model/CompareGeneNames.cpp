/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QString>
#include <QDebug>
#include "SortGenesProxyModel.h"

SortGenesProxyModel::SortCategory sortCategory(const QString &name)
{
    if (name.size() == 0) {
        return SortGenesProxyModel::numericGene;
    }
    if (!name[0].isLetter()) {
        return SortGenesProxyModel::numericGene;
    }
    if (name.startsWith("ambiguous")) {
        return SortGenesProxyModel::ambiguousGene;
    }

    return SortGenesProxyModel::normalGene;
}

bool geneNameLessThan(const QString &geneName1, const QString &geneName2,
                      const Qt::CaseSensitivity &caseSensitivity, bool isSortLocaleAware)
{
    SortGenesProxyModel::SortCategory geneName1_category = sortCategory(geneName1);
    SortGenesProxyModel::SortCategory geneName2_category = sortCategory(geneName2);
    if (geneName1_category == geneName2_category) {
        if (isSortLocaleAware) {
            return geneName1.localeAwareCompare(geneName2) < 0;
        } else {
            return geneName1.compare(geneName2, caseSensitivity) < 0;
        }
    }

    return geneName2_category < geneName1_category;
}
