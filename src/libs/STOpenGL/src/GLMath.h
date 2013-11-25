/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLMATH_H
#define GLMATH_H

namespace GL
{

template <typename T>
inline const T min(const T &a, const T &b);
template <typename T>
inline const T min(const T &a, const T &b, const T &c);
template <typename T>
inline const T max(const T &a, const T &b);
template <typename T>
inline const T max(const T &a, const T &b, const T &c);

template <typename T>
inline const T clamp(const T &x, const T &lo, const T &hi);

} // namespace GL //

/****************************************** DEFINITION ******************************************/

namespace GL
{

template <typename T>
inline const T min(const T &a, const T &b)
{
    return (b < a) ? b : a;
}
template <typename T>
inline const T min(const T &a, const T &b, const T &c)
{
    return min(a, min(b, c));
}
template <typename T>
inline const T max(const T &a, const T &b)
{
    return (b > a) ? b : a;
}
template <typename T>
inline const T max(const T &a, const T &b, const T &c)
{
    return max(a, max(b, c));
}
template <typename T>
inline const T clamp(const T &x, const T &lo, const T &hi)
{
    return max(lo, min(hi, x));
}

} // namespace GL //

#endif // GLMATH_H //
