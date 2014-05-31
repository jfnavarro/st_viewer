/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef COMPAREGENENAMES_H
#define COMPAREGENENAMES_H

class QString;

/* When we sort a table of gene names we would like to place the normal gene names first.
   After that we place gene names that does not start with a letter.
   Gene names containing the word "ambigous" is placed at the very last end of the list.
*/

bool geneNameLessThan(const QString &geneName1, const QString &geneName2,
                      const Qt::CaseSensitivity &caseSensitivity, bool isSortLocaleAware);


#endif // COMPAREGENENAMES_H
