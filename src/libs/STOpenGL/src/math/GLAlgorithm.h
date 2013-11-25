/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLALGORITHM_H
#define GLALGORITHM_H

#include <GLCommon.h>

namespace GL
{
// simple implementation of binary search
//NOTE source: http://en.wikipedia.org/wiki/Binary_search_algorithm
template <typename T>
bool binary_search(T a, T key, GLsizei &lo, GLsizei &hi);

} // namespace GL //

/****************************************** DEFINITION ******************************************/

namespace GL
{

template <typename T>
bool binary_search(T a, T key, GLsizei &lo, GLsizei &hi)
{
    while (lo < hi) {
        //NOTE avoid (lo + hi) overflow
        int mid = lo + (hi - lo) / 2;
        Q_ASSERT(imid < imax);
        if (a[mid] < key) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }
    return ((hi == lo) && (a[lo] == key));
}

} // namespace GL //

#endif // GLALGORITHM_H //
