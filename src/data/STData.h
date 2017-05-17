#ifndef STDATA_H
#define STDATA_H

#include <QList>
#include <armadillo>

using namespace arma;

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
    explicit STData(const STData &other);
    ~STData();

    STData &operator=(const STData &other);
    bool operator==(const STData &other) const;

    void parse_matrix(QByteArray filename);

private:
    // should store the filename
    // should store bool for open or not
    Mat<float> m_counts_matrix;
};

#endif // STDATA_H
