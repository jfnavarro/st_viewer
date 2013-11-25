/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLINPLACE_H
#define GLINPLACE_H

#include "GLCommon.h"

namespace GL
{
// Simple inplace array. GLInplaceArray provides a fixed size array
// which allocates all data in one single chunk. Designed to be used
// where in place memory allocation is important.
template <typename T, int S>
class GLInplaceArray
{
public:
    typedef T key_type;
    typedef GLsizei size_type;

    inline GLInplaceArray();
    inline explicit GLInplaceArray(GL::Initialization);

    inline const GLsizei size() const;
    inline const GLsizei capacity() const;

    inline void clear();

    inline const bool isEmpty() const;
    inline const bool isFull() const;

    inline T &operator[](const int i);
    inline const T &operator[](const int i) const;

    inline const bool pushBack(const T &t);
    inline const bool popBack();

    static void fill(T(&tS)[S], const T &t);
    static void copy(T(&aS)[S], const T(&bS)[S]);

private:
    GLsizei m_size;
    T m_data[S];
};

} // namespace GL //

/****************************************** DEFINITION ******************************************/

namespace GL
{

template <typename T, int S>
inline GLInplaceArray<T, S>::GLInplaceArray()
    : m_size(0)
{
    //TODO initialize array (if default constructor exists)
}
template <typename T, int S>
inline GLInplaceArray<T, S>::GLInplaceArray(GL::Initialization)
    : m_size(0)
{

}

template <typename T, int S>
inline const GLsizei GLInplaceArray<T, S>::size() const
{
    return m_size;
}
template <typename T, int S>
inline const GLsizei GLInplaceArray<T, S>::capacity() const
{
    return GLsizei(S);
}

template <typename T, int S>
inline void GLInplaceArray<T, S>::clear()
{
    m_size = 0;
}

template <typename T, int S>
inline const bool GLInplaceArray<T, S>::isEmpty() const
{
    return (size() == 0);
}
template <typename T, int S>
inline const bool GLInplaceArray<T, S>::isFull() const
{
    return (size() == capacity());
}

template <typename T, int S>
inline T &GLInplaceArray<T, S>::operator[](const int i)
{
    return m_data[i];
}
template <typename T, int S>
inline const T &GLInplaceArray<T, S>::operator[](const int i) const
{
    return m_data[i];
}

template <typename T, int S>
inline const bool GLInplaceArray<T, S>::pushBack(const T &t)
{
    if (isFull()) {
        return false;
    }
    m_data[m_size++] = t;
    return true;
}

template <typename T, int S>
inline const bool GLInplaceArray<T, S>::popBack()
{
    if (isEmpty()) {
        return false;
    }
    m_data[--m_size];
    return true;
}

template <typename T, int S>
void GLInplaceArray<T, S>::fill(T(&tS)[S], const T &t)
{
    for (int i = 0; i < S; ++i) {
        tS[i] = t;
    }
}

template <typename T, int S>
void GLInplaceArray<T, S>::copy(T(&aS)[S], const T(&bS)[S])
{
    memcpy(aS, bS, sizeof(aS));
}

} // namespace GL //

#endif // GLINPLACE_H //
