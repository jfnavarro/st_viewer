/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef QUADTREE_H
#define QUADTREE_H

#include <QDebug>
#include <QVector2D>

#include <vector>
#include <array>

#include "Common.h"
#include "QuadTreeAABB.h"

// Simple template based quad tree implementation using QPointF as lookup
// method. Each point is associated with data of type T. The N argument
// determines the maximum number of items within a bucket before it is
// split further.

// NOTE this implementation does not allow multiple data to be stored in the
// same point (this is to avoid inf-recursion when splitting).

template <typename T, int N = 8>
class QuadTree
{
public:
    typedef QPair<QPointF, T> PointItem;
    typedef QVector<PointItem> PointItemList;
    typedef QVector<QuadTreeAABB> BoundingBoxList;

    QuadTree();
    explicit QuadTree(const QSizeF& size);
    explicit QuadTree(const QuadTreeAABB& boundingBox);
    explicit QuadTree(const QRectF& rect);
    ~QuadTree();

    bool contains(const QPointF& p) const;

    // insert new data at point p returning true if  data was successfully
    // inserted (no data exists on that point).
    bool insert(const QPointF& p, const T& t);

    // return a list of all items within the given area
    void select(const QuadTreeAABB& b, PointItemList& items) const;

    // return the item at the specified point
    void select(const QPointF& p, PointItem& item) const;

    // clean up
    void clear();

    int buckets() const;
    int bucketCapacity() const;

    void boundingBoxList(BoundingBoxList& buckets) const;

private:
    int insert_p(const QPointF& p, const T& t, const int idx);
    void smash(const int idx);

    // Simple representation of a quad tree bucket.
    struct Bucket {
        static const int INSERT_OK = -1;
        static const int INSERT_ERROR_FULL = -2;
        static const int INSERT_ERROR_NONUNIQUE = -3;
        static const int LOOKUP_FOUND = -1;
        static const int LOOKUP_NOT_FOUND = -2;
        static const int POINT_LIST_LIMIT = N;

        typedef QVector<PointItem> StaticPointItemList;
        typedef std::array<int, 4> QuadArrayType;

        Bucket();
        explicit Bucket(const QuadTreeAABB& b);
        ~Bucket();

        bool contains(const QPointF& p) const;

        int insert(const QPointF& p, const T& t);

        void select(const QuadTreeAABB& b, PointItemList& items, QuadArrayType& point_array) const;

        void select(const QPointF& p, PointItem& item, QuadArrayType& point_array) const;

        bool isNode() const;
        bool isLeaf() const;

        QuadTreeAABB aabb;
        QuadArrayType quads;
        StaticPointItemList data; // store all data as part of bucket struct
    };

    typedef QVector<Bucket> BucketList;
    BucketList m_data;
};

/****************************************** DEFINITION ******************************************/

template <typename T, int N>
void QuadTree<T, N>::boundingBoxList(BoundingBoxList& buckets) const
{
    typename BucketList::const_iterator it = m_data.begin();
    typename BucketList::const_iterator end = m_data.end();
    for (; it != end; ++it) {
        if (it->isLeaf()) {
            buckets.push_back(it->QuadTreeAABB);
        }
    }
}

template <typename T, int N>
void QuadTree<T, N>::clear()
{
    m_data.clear();
}

template <typename T, int N>
void QuadTree<T, N>::Bucket::select(const QuadTreeAABB& b,
                                    PointItemList& items,
                                    QuadArrayType& point_array) const
{
    // early out
    if (!aabb.intersects(b)) {
        // NOTE explicitly define const value variable to avoid weird bug on Mac
        const int value = LOOKUP_NOT_FOUND;
        std::fill(point_array.begin(), point_array.end(), value);
        return;
    }

    // if none-leaf node (ie. no data)
    if (isNode()) {
        std::copy(quads.begin(), quads.end(), point_array.begin());
        return;
    }

    // else add data selected
    // add all items if bucket contained (speed up)
    const typename StaticPointItemList::size_type size = data.size();
    if (b.contains(aabb)) {
        for (typename StaticPointItemList::size_type i = 0; i < size; ++i) {
            items.push_back(data[i]);
        }
    }
    // else test and add individual items
    else {
        for (typename StaticPointItemList::size_type i = 0; i < size; ++i) {
            if (b.contains(data[i].first)) {
                items.push_back(data[i]);
            }
        }
    }
    // NOTE explicitly define const value variable to avoid weird bug on Mac
    const int value = LOOKUP_FOUND;
    std::fill(point_array.begin(), point_array.end(), value);
}

template <typename T, int N>
void QuadTree<T, N>::Bucket::select(const QPointF& p,
                                    PointItem& item,
                                    QuadArrayType& point_array) const
{
    // early out
    if (!aabb.contains(p)) {
        // NOTE explicitly define const value variable to avoid weird bug on Mac
        const int value = LOOKUP_NOT_FOUND;
        std::fill(point_array.begin(), point_array.end(), value);
        return;
    }

    // if none-leaf node (ie. no data)
    if (isNode()) {
        std::copy(quads.begin(), quads.end(), point_array.begin());
        return;
    }

    // else add data selected
    // test and add individual item
    const typename StaticPointItemList::size_type size = data.size();
    for (typename StaticPointItemList::size_type i = 0; i < size; ++i) {
        if (STMath::qFuzzyEqual(p, data[i].first)) {
            item = data[i];
        }
    }

    // NOTE explicitly define const value variable to avoid weird bug on Mac
    const int value = LOOKUP_FOUND;
    std::fill(point_array.begin(), point_array.end(), value);
}

template <typename T, int N>
int QuadTree<T, N>::Bucket::insert(const QPointF& p, const T& t)
{
    static const unsigned table[] = {0u, 1u, 3u, 2u};

    // if non-leaf bucket
    if (quads[0] >= 0) {
        const QPointF middle_point = aabb.middle();
        const QVector2D middle_vector(p.x() - middle_point.x(), p.y() - middle_point.y());
        const unsigned idx
            = ((middle_vector.x() < 0.0) ? 1u : 0u) + ((middle_vector.y() < 0.0) ? 2u : 0u);
        const unsigned q = table[idx];
        return quads[q];
    }

    // DEBUG force p to be unique to avoid inf recursion!
    typename StaticPointItemList::size_type size = data.size();
    for (typename StaticPointItemList::size_type i = 0; i < size; ++i) {
        if (STMath::qFuzzyEqual(data[i].first, p)) {
            return INSERT_ERROR_NONUNIQUE;
        }
    }

    // try to insert
    if (size == POINT_LIST_LIMIT) {
        return INSERT_ERROR_FULL;
    } else {
        PointItem pair(p, t);
        data.push_back(pair);
        return INSERT_OK;
    }
}

template <typename T, int N>
int QuadTree<T, N>::insert_p(const QPointF& p, const T& t, const int idx)
{
    // insert or smash on full
    int lastIdx;
    int currIdx = idx;
    while ((lastIdx = currIdx) >= 0) {
        currIdx = m_data[currIdx].insert(p, t);
        if (currIdx == Bucket::INSERT_ERROR_FULL) {
            // smash bucket and retry insert (recycle index)
            smash((currIdx = lastIdx));
        }
    }
    return lastIdx;
}

// GLQuadTree
template <typename T, int N>
QuadTree<T, N>::QuadTree()
    : m_data()
{
}

template <typename T, int N>
QuadTree<T, N>::~QuadTree()
{
}

template <typename T, int N>
QuadTree<T, N>::QuadTree(const QSizeF& size)
    : m_data()
{
    const QuadTreeAABB boundingBox = QuadTreeAABB(0.0, 0.0, size.width(), size.height());
    m_data.push_back(Bucket(boundingBox));
}

template <typename T, int N>
QuadTree<T, N>::QuadTree(const QuadTreeAABB& boundingBox)
    : m_data()
{
    m_data.push_back(Bucket(boundingBox));
}

template <typename T, int N>
QuadTree<T, N>::QuadTree(const QRectF& rect)
    : m_data()
{
    const QuadTreeAABB boundingBox = QuadTreeAABB(rect);
    m_data.push_back(Bucket(boundingBox));
}

template <typename T, int N>
bool QuadTree<T, N>::insert(const QPointF& p, const T& t)
{
    // early out
    if (!contains(p)) {
        return false;
    }
    int idx = insert_p(p, t, 0);
    return (idx == Bucket::INSERT_OK);
}

template <typename T, int N>
bool QuadTree<T, N>::contains(const QPointF& p) const
{
    // only look root bucket since it by definition contains all others
    return (m_data.empty() ? false : m_data[0].contains(p));
}

template <typename T, int N>
void QuadTree<T, N>::select(const QuadTreeAABB& b, PointItemList& items) const
{
    typedef QVector<int> IndexList;
    IndexList indicies;

    if (!m_data.empty()) {
        indicies.push_back(0);
    }

    while (!indicies.empty()) {
        const int idx = indicies.back();
        indicies.pop_back();

        typename Bucket::QuadArrayType ret;
        m_data[idx].select(b, items, ret);

        if (ret[0] >= 0) {
            indicies.push_back(ret[0]);
        }
        if (ret[1] >= 0) {
            indicies.push_back(ret[1]);
        }
        if (ret[2] >= 0) {
            indicies.push_back(ret[2]);
        }
        if (ret[3] >= 0) {
            indicies.push_back(ret[3]);
        }
    }
}

template <typename T, int N>
void QuadTree<T, N>::select(const QPointF& p, PointItem& item) const
{
    typedef std::vector<int> IndexList;
    IndexList indicies;

    if (!m_data.empty()) {
        indicies.push_back(0);
    }

    while (!indicies.empty()) {
        const int idx = indicies.back();
        indicies.pop_back();

        typename Bucket::QuadArrayType ret;
        m_data[idx].select(p, item, ret);

        if (ret[0] >= 0) {
            indicies.push_back(ret[0]);
        }
        if (ret[1] >= 0) {
            indicies.push_back(ret[1]);
        }
        if (ret[2] >= 0) {
            indicies.push_back(ret[2]);
        }
        if (ret[3] >= 0) {
            indicies.push_back(ret[3]);
        }
    }
}

template <typename T, int N>
int QuadTree<T, N>::buckets() const
{
    return m_data.size();
}

template <typename T, int N>
int QuadTree<T, N>::bucketCapacity() const
{
    return N;
}

template <typename T, int N>
void QuadTree<T, N>::smash(const int idx)
{
    // NOTE reallocation of m_data can be triggered at multiple points in
    //     this function (marked with [*]) so make sure NOT to to use any
    //     locally cached references!

    const int newIdxHead = m_data.size();

    // create 4 new buckets
    m_data.push_back(Bucket(m_data[idx].aabb.split(QuadTreeAABB::Q0))); // [*]
    m_data.push_back(Bucket(m_data[idx].aabb.split(QuadTreeAABB::Q1))); // [*]
    m_data.push_back(Bucket(m_data[idx].aabb.split(QuadTreeAABB::Q2))); // [*]
    m_data.push_back(Bucket(m_data[idx].aabb.split(QuadTreeAABB::Q3))); // [*]

    // link parent (assumes allocation is sequential)
    const typename Bucket::QuadArrayType newIdxList
        = {{newIdxHead + 0, newIdxHead + 1, newIdxHead + 2, newIdxHead + 3}};
    std::copy(newIdxList.begin(), newIdxList.end(), m_data[idx].quads.begin());

    // reinsert data
    const typename Bucket::StaticPointItemList::size_type size = m_data[idx].data.size();
    for (typename Bucket::StaticPointItemList::size_type i = 0; i < size; ++i) {
        insert_p(m_data[idx].data[i].first, m_data[idx].data[i].second, idx); // [*]
    }

    // remove data from parent
    m_data[idx].data.clear();
}

// GLQuadTree::Bucket
template <typename T, int N>
QuadTree<T, N>::Bucket::Bucket()
    : aabb()
    , data()
{
    std::fill(quads.begin(), quads.end(), -1);
}

template <typename T, int N>
QuadTree<T, N>::Bucket::~Bucket()
{
}

template <typename T, int N>
QuadTree<T, N>::Bucket::Bucket(const QuadTreeAABB& b)
    : aabb(b)
    , data()
{
    std::fill(quads.begin(), quads.end(), -1);
}

template <typename T, int N>
bool QuadTree<T, N>::Bucket::contains(const QPointF& p) const
{
    return aabb.contains(p);
}

template <typename T, int N>
bool QuadTree<T, N>::Bucket::isNode() const
{
    return (quads[0] >= 0 || quads[1] >= 0 || quads[2] >= 0 || quads[3] >= 0);
}

template <typename T, int N>
bool QuadTree<T, N>::Bucket::isLeaf() const
{
    return (quads[0] < 0 && quads[1] < 0 && quads[2] < 0 && quads[3] < 0);
}

#endif // QUADTREE_H //
