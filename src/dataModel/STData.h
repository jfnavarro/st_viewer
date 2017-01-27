#ifndef STDATA_H
#define STDATA_H

#include <vector>

class STData
{

public:

    typedef std::pair<float,float> spot_type;
    typedef std::string gene_type;
    typedef std::vector<spot_type> spot_list;
    typedef std::vector<gene_type> gene_list;

    STData();
};

#endif // STDATA_H
