/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLMATH_H
#define GLMATH_H

#include <qmath.h>

// useful defines
#ifndef M_SQRT3
#define M_SQRT3     1.73205080756887729353  /* sqrt(3) */
#endif // M_SQRT3 //

namespace GL
{

// basic math functions
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

// normalization functions
// maps the value v in [t0,t1] to the normalized value vn in [0,1]
template <typename T, typename R>
const R norm(const T v, const T t0, const T t1);

// maps the normalized value vn in [0,1] to the value v in [t0,t1]
template <typename T, typename R>
const T denorm(const R vn, const T t0, const T t1);

// basic step functions
// round number down to last multiple of multiple
template <typename T>
inline const T lastMultiple(const T &number, const T &multiple);
// rounds number up to next multiple of multiple
template <typename T>
inline const T nextMultiple(const T &number, const T &multiple);

// relational operators
template <typename T>
struct comp_op_ls { // less <
    static inline bool compare(const T &a, const T &b);
};

template <typename T>
struct comp_op_le { // less or equal <=
    static inline bool compare(const T &a, const T &b);
};

template <typename T>
struct comp_op_gt { // greater >
    static inline bool compare(const T &a, const T &b);
};

template <typename T>
struct comp_op_ge { // greater or equal >=
    static inline bool compare(const T &a, const T &b);
};

// test for range (using template based range, default [lo, hi], ie. (lo <= v <= hi))
template <typename T, typename OP_LO = comp_op_ge<T>, typename OP_HI = comp_op_le<T> >
struct range {
    static inline bool compare(const T &v, const T &lo, const T &hi);
};

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

template <typename T, typename R>
const R norm(const T v, const T t0, const T t1)
{
    const T vh = clamp(v, t0, t1);
    return R(vh - t0) / R(t1 - t0);
}

template <typename T, typename R>
const T denorm(const R nv, const T t0, const T t1)
{
    const R vh = clamp(nv, R(0.0), R(1.0));
    return T(vh * (t1 - t0)) + t0;
}

template <typename T>
inline const T lastMultiple(const T &number, const T &multiple)
{
    const T remainder = number % multiple;
    return (number - remainder);
}
template <typename T>
inline const T nextMultiple(const T &number, const T &multiple)
{
    const T remainder = number % multiple;
    return (number + multiple - remainder);
}

template <typename T>
inline bool comp_op_ls<T>::compare(const T &a, const T &b)
{
    return (a < b);
}

template <typename T>
inline bool comp_op_le<T>::compare(const T &a, const T &b)
{
    return (a <= b);
}

template <typename T>
inline bool comp_op_gt<T>::compare(const T &a, const T &b)
{
    return (a > b);
}

template <typename T>
inline bool comp_op_ge<T>::compare(const T &a, const T &b)
{
    return (a >= b);
}

template <typename T, typename OP_LO, typename OP_HI>
inline bool range<T, OP_LO, OP_HI>::compare(const T &v, const T &lo, const T &hi)
{
    return OP_LO::compare(v, lo) && OP_HI::compare(v, hi);
}

} // namespace GL //

#endif // GLMATH_H //
