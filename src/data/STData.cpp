#include "STData.h"

STData::STData()
{
}

STData::STData(int dummy):dummy(dummy)
{
}

STData::STData(const STData &other)
{
    dummy = other.dummy;
}

STData::~STData()
{
}

STData &STData::operator=(const STData &other)
{
    dummy = other.dummy;
    return (*this);
}

bool STData::operator==(const STData &other) const
{
    return dummy == other.dummy;
}
