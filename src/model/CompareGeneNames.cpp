/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QString>
#include <QDebug>

enum SortCategory {AMBIGOUS_GENE_NAME = 0,
                   GENE_NAME_DOES_NOT_START_WITH_LETTER = 1,
                   NORMAL_GENE_NAME = 2};

SortCategory sortCategory(const QString &name) {
    if (name.size() == 0) {
        return GENE_NAME_DOES_NOT_START_WITH_LETTER;
    }
    if (! name[0].isLetter()) {
        return GENE_NAME_DOES_NOT_START_WITH_LETTER;
    }
    if (name.startsWith("ambiguous")) {
        return AMBIGOUS_GENE_NAME;
    }
    return NORMAL_GENE_NAME;
}

bool geneNameLessThan(const QString &geneName1, const QString &geneName2,
                      const Qt::CaseSensitivity &caseSensitivity, bool isSortLocaleAware)
{
    SortCategory geneName1_category = sortCategory(geneName1);
    SortCategory geneName2_category = sortCategory(geneName2);    
    if (geneName1_category == geneName2_category) {
        if (isSortLocaleAware) {
	  return geneName1.localeAwareCompare(geneName2) < 0;
	} else {
           return geneName1.compare(geneName2, caseSensitivity) < 0;
	}
    }
    bool res = geneName2_category < geneName1_category;
    return res;
}
