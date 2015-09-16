/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef TST_QUADTREETEST_H
#define TST_QUADTREETEST_H

#include <QObject>
#include <QList>

#include "math/QuadTree.h"

namespace unit
{

class GLQuadTreeTest : public QObject
{
    Q_OBJECT

public:
    explicit GLQuadTreeTest(QObject* parent = 0);

private:
    typedef QList<QPointF> PointList;
    typedef QuadTree<int, 4> TestQuadTree;

private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();

    void testInsert();
    void testInsert_data();
};

} // namespace unit //

#endif // TST_QUADTREETEST_H //
