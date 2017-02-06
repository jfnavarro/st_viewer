#ifndef STDATA_H
#define STDATA_H

#include <QList>

class STData
{

public:

    typedef QPair<float,float> spot_type;
    typedef QString gene_type;
    typedef QList<spot_type> spot_list;
    typedef QList<gene_type> gene_list;
    typedef QPair<gene_type, float> gene_count;
    typedef QPair<spot_type, float> spot_count;
    typedef QList<spot_count> spot_count_list;
    typedef QList<gene_count> gene_count_list;

    STData();
};

#endif // STDATA_H
