#ifndef STDATA_H
#define STDATA_H

#include <QList>
class Spot;
class Gene;

class STData
{

public:

    typedef QSharedPointer<Spot> spot_type;
    typedef QSharedPointer<Gene> gene_type;
    typedef QList<spot_type> spot_list;
    typedef QList<gene_type> gene_list;
    //typedef QPair<gene_type, float> gene_count;
    //typedef QPair<spot_type, float> spot_count;
    //typedef QList<spot_count> spot_count_list;
    //typedef QList<gene_count> gene_count_list;

    STData();
    STData(int dummy);
    explicit STData(const STData &other);
    ~STData();

    STData &operator=(const STData &other);
    bool operator==(const STData &other) const;

private:
    int dummy;
};

#endif // STDATA_H
