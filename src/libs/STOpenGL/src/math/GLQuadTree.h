/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLQUADTREE_H
#define GLQUADTREE_H

#include <QDebug>
#include <GLQt.h>

#include <vector>

#include <GLCommon.h>
#include <GLInplace.h>
#include <math/GLAABB.h>
#include <math/GLVector.h>

namespace GL
{
    // Simple template based quad tree implementation using GLpoints as lookup
    // method. Each point is associated with data of type T. The N argument
    // determines the maximum number of items within a bucket before it is
    // split further.
    //NOTE this implementation does not allow multiple data to be stored in the
    // same point (this is to avoid inf-recursion when splitting).
    template <typename T, int N = 8>
    class GLQuadTree
    {
    public:
        typedef GLpair<GLpoint, T> PointItem;
        typedef std::vector<PointItem> PointItemList;
        typedef std::vector<GLaabb> BoundingBoxList;

        GLQuadTree();
        GLQuadTree(const GLpoint &size);
        GLQuadTree(const GLaabb &boundingBox);

        const bool contains(const GLpoint &p) const;

        // insert new data at point p returning true iff data was successfully
        // inserted (no data exists on that point).
        const bool insert(const GLpoint &p, const T &t);
        // return a list of all items within the given area
        void select(const GLaabb &b, PointItemList &items) const;
        // return the item at the specified point
        void select(const GLpoint &p, PointItem &item) const;

        void clear();

        const GLint buckets() const;
        const GLint bucketCapacity() const;
        void boundingBoxList(BoundingBoxList &buckets) const;

    private:

        const GLint insert_p(const GLpoint &p, const T &t, const GLint idx);
        void smash(const GLint idx);

        // Simple representation of a quad tree bucket.
        struct Bucket
        {
             //static const GLint INSERT_OK = GLint(-1);
             //static const GLint INSERT_ERROR_FULL = GLint(-2);
             //static const GLint INSERT_ERROR_NONUNIQUE = GLint(-3);
             //static const GLint LOOKUP_FOUND = GLint(-1);
             //static const GLint LOOKUP_NOT_FOUND = GLint(-2);

            Bucket();
            Bucket(const GLaabb &aabb);

            const bool contains(const GLpoint &p) const;
            const GLint insert(const GLpoint &p, const T &t);
            void select(const GLaabb &b, PointItemList &items, GLint (&idx)[4]) const;
            void select(const GLpoint &p, PointItem &item, GLint (&idx)[4]) const;

            const bool isNode() const;
            const bool isLeaf() const;

            typedef GLInplaceArray<PointItem,N> StaticPointItemList;
            GLaabb aabb;
            GLint quads[4];
            StaticPointItemList data; // store all data as part of bucket struct
        };

        // inplace memory allocation, all data stored sequentially in memory
        typedef std::vector<Bucket> BucketList;
        BucketList m_data;
    };
    //TODO should really make this guy partof Bucket
    static const GLint INSERT_OK = GLint(-1);
    static const GLint INSERT_ERROR_FULL = GLint(-2);
    static const GLint INSERT_ERROR_NONUNIQUE = GLint(-3);
    static const GLint LOOKUP_FOUND = GLint(-1);
    static const GLint LOOKUP_NOT_FOUND = GLint(-2);

} // namespace GL //

/****************************************** DEFINITION ******************************************/

namespace GL
{
    
    template <typename T, int N>
    void GLQuadTree<T,N>::boundingBoxList(BoundingBoxList &buckets) const
    {
        typename BucketList::const_iterator it, end = m_data.end();
        for (it = m_data.begin(); it != end; ++it)
        {
            if (it->isLeaf())
            {
                buckets.push_back(it->aabb);
            }
        }
    }
    
    template <typename T, int N>
    void GLQuadTree<T,N>::clear()
    {
        m_data.clear();
    }
    
    template <typename T, int N>
    void GLQuadTree<T,N>::Bucket::select(const GLaabb &b, PointItemList &items, GLint (&idx)[4]) const
    {
        // early out
        if (!aabb.intersects(b))
        {
            //NOTE explicitly define const value variable to avoid weird bug on Mac
            const GLint value = LOOKUP_NOT_FOUND;
            GLInplaceArray<GLint,4>::fill(idx, value);
            return;
        }
        
        // if none-leaf node (ie. no data)
        if (isNode())
        {
            GLInplaceArray<GLint,4>::copy(idx, quads);
            return;
        }
        
        // else add data selected
        
        // add all items if bucket contained (speed up)
        if (b.contains(aabb))
        {
            typename StaticPointItemList::size_type size = data.size();
            for (typename StaticPointItemList::size_type i = 0; i<size; ++i)
            {
                items.push_back(data[i]);
            }
        }
        // else test and add individual items
        else
        {
            const typename StaticPointItemList::size_type size = data.size();
            for (typename StaticPointItemList::size_type i = 0; i<size; ++i)
            {
                if (b.contains(data[i].first))
                {
                    items.push_back(data[i]);
                }
            }
        }
        
        //NOTE explicitly define const value variable to avoid weird bug on Mac
        const GLint value = LOOKUP_FOUND;
        GLInplaceArray<GLint,4>::fill(idx, value);
    }
    
    template <typename T, int N>
    void GLQuadTree<T,N>::Bucket::select(const GLpoint &p, PointItem &item, GLint (&idx)[4]) const
    {
        // early out
        if (!aabb.contains(p))
        {
            //NOTE explicitly define const value variable to avoid weird bug on Mac
            const GLint value = LOOKUP_NOT_FOUND;
            GLInplaceArray<GLint,4>::fill(idx, value);
            return;
        }
        
        // if none-leaf node (ie. no data)
        if (isNode())
        {
            GLInplaceArray<GLint,4>::copy(idx, quads);
            return;
        }
        
        // else add data selected
        
        // test and add individual item
        const typename StaticPointItemList::size_type size = data.size();
        for (typename StaticPointItemList::size_type i = 0; i<size; ++i)
        {
            if (fuzzyEqual(p, data[i].first))
            {
                item = data[i];
            }
        }
        
        //NOTE explicitly define const value variable to avoid weird bug on Mac
        const GLint value = LOOKUP_FOUND;
        GLInplaceArray<GLint,4>::fill(idx, value);
        
    }
    
    template <typename T, int N>
    const GLint GLQuadTree<T,N>::Bucket::insert(const GLpoint &p, const T &t)
    {
        // if non-leaf bucket
        if (quads[0] >= 0)
        {
            const GLuint q = GLvector::fromPoints(aabb.middle(), p).quadrant();
            return quads[q];
        }
        
        //DEBUG force p to be unique to avoid inf recursion!
        typename StaticPointItemList::size_type size = data.size();
        for (typename StaticPointItemList::size_type i = 0; i<size; ++i)
        {
            if (fuzzyEqual(data[i].first, p))
            {
                //qDebug() << "GLQuadTree: Error! Trying to insert non-unique point!";
                return INSERT_ERROR_NONUNIQUE;
            }
        }
        
        // try to insert
        PointItem pair = { p, t };
        return data.pushBack(pair) ? INSERT_OK : INSERT_ERROR_FULL;
    }
    
    template <typename T, int N>
    const GLint GLQuadTree<T,N>::insert_p(const GLpoint &p, const T &t, const GLint idx)
    {
        // insert or smash on full
        GLint lastIdx, currIdx = idx;
        while ((lastIdx = currIdx) >= 0)
        {
            currIdx = m_data[currIdx].insert(p, t);
            if (currIdx == INSERT_ERROR_FULL)
            {
                // smash bucket and retry insert (recycle index)
                smash((currIdx = lastIdx));
            }
        }
        
        return lastIdx;
    }
    
    // GLQuadTree
    template <typename T, int N>
    GLQuadTree<T,N>::GLQuadTree()
    : m_data()
    {
        
    }
    template <typename T, int N>
    GLQuadTree<T,N>::GLQuadTree(const GLpoint &size)
    : m_data()
    {
        const GLaabb boundingBox = GLaabb(0.0f, 0.0f, size.width, size.height);
        m_data.push_back(Bucket(boundingBox));
    }
    template <typename T, int N>
    GLQuadTree<T,N>::GLQuadTree(const GLaabb &boundingBox)
    : m_data()
    {
        m_data.push_back(Bucket(boundingBox));
    }
    
    template <typename T, int N>
    const bool GLQuadTree<T,N>::insert(const GLpoint &p, const T &t)
    {
        // early out
        if (!contains(p))
        {
            return false;
        }
        
        GLint idx = insert_p(p, t, 0);
        return (idx == INSERT_OK);
    }
    
    template <typename T, int N>
    const bool GLQuadTree<T,N>::contains(const GLpoint &p) const
    {
        // only look root bucket since it by definition contains all others
        return (m_data.empty() ? false : m_data[0].contains(p));
    }
    
    template <typename T, int N>
    void GLQuadTree<T,N>::select(const GLaabb &b, PointItemList &items) const
    {
        typedef std::vector<GLint> IndexList;
        IndexList indicies;
        
        if (!m_data.empty()) {
            indicies.push_back(0);
        }
        
        while (!indicies.empty())
        {
            const GLint idx = indicies.back();
            indicies.pop_back();
            
            GLint ret[4];
            m_data[idx].select(b, items, ret);
            if (ret[0] >= 0) { indicies.push_back(ret[0]); }
            if (ret[1] >= 0) { indicies.push_back(ret[1]); }
            if (ret[2] >= 0) { indicies.push_back(ret[2]); }
            if (ret[3] >= 0) { indicies.push_back(ret[3]); }
        }
    }
    
    template <typename T, int N>
    void GLQuadTree<T,N>::select(const GLpoint &p, PointItem &item) const
    {
        typedef std::vector<GLint> IndexList;
        IndexList indicies;
        
        if (!m_data.empty()) {
            indicies.push_back(0);
        }
        
        while (!indicies.empty())
        {
            const GLint idx = indicies.back();
            indicies.pop_back();
            
            GLint ret[4];
            m_data[idx].select(p, item, ret);
            if (ret[0] >= 0) { indicies.push_back(ret[0]); }
            if (ret[1] >= 0) { indicies.push_back(ret[1]); }
            if (ret[2] >= 0) { indicies.push_back(ret[2]); }
            if (ret[3] >= 0) { indicies.push_back(ret[3]); }
        }
    }
    
    
    template <typename T, int N>
    const GLint GLQuadTree<T,N>::buckets() const
    {
        return GLint(m_data.size());
    }
    
    template <typename T, int N>
    const GLint GLQuadTree<T,N>::bucketCapacity() const
    {
        return GLint(N);
    }
    
    template <typename T, int N>
    void GLQuadTree<T,N>::smash(const GLint idx)
    {
        //NOTE reallocation of m_data can be triggered at multiple points in
        //     this function (marked with [*]) so make sure NOT to to use any
        //     locally cached references!
        
        const GLint newIdxHead = static_cast<GLint>(m_data.size());
        
        // create 4 new buckets
        m_data.push_back(Bucket(m_data[idx].aabb.split(GLaabb::Q0))); // [*]
        m_data.push_back(Bucket(m_data[idx].aabb.split(GLaabb::Q1))); // [*]
        m_data.push_back(Bucket(m_data[idx].aabb.split(GLaabb::Q2))); // [*]
        m_data.push_back(Bucket(m_data[idx].aabb.split(GLaabb::Q3))); // [*]
        
        // link parent (assumes allocation is sequential)
        const GLint newIdxList[4] = { newIdxHead + 0, newIdxHead + 1, newIdxHead + 2, newIdxHead + 3 };
        GLInplaceArray<GLint,4>::copy(m_data[idx].quads, newIdxList);
        
        // reinsert data
        const typename Bucket::StaticPointItemList::size_type size = m_data[idx].data.size(); 
        for (typename Bucket::StaticPointItemList::size_type i=0; i<size; ++i)
        {
            insert_p(m_data[idx].data[i].first, m_data[idx].data[i].second, idx); // [*]
        }
        
        // remove data from parent
        m_data[idx].data.clear();
    }
    
    // GLQuadTree::Bucket
    template <typename T, int N>
    GLQuadTree<T,N>::Bucket::Bucket()
    : aabb(), data(GL::Uninitialized)
    {
        GLInplaceArray<GLint,4>::fill(quads, -1);
    }
    template <typename T, int N>
    GLQuadTree<T,N>::Bucket::Bucket(const GLaabb &aabb)
    : aabb(aabb), data(GL::Uninitialized)
    {
        GLInplaceArray<GLint,4>::fill(quads, -1);
    }
    
    
    
    template <typename T, int N>
    const bool GLQuadTree<T,N>::Bucket::contains(const GLpoint &p) const
    {
        return aabb.contains(p);
    }
    
    
    
    template <typename T, int N>
    const bool GLQuadTree<T,N>::Bucket::isNode() const
    {
        return (quads[0] >= 0 || quads[1] >= 0 || quads[2] >= 0 || quads[3] >= 0);
    }
    
    template <typename T, int N>
    const bool GLQuadTree<T,N>::Bucket::isLeaf() const
    {
        return (quads[0] < 0 && quads[1] < 0 && quads[2] < 0 && quads[3] < 0);
    }

} // namespace GL //

#endif // GLQUADTREE_H //
