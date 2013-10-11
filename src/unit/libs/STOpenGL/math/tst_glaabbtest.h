/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef TST_GLAABBTEST_H
#define TST_GLAABBTEST_H

#include <QObject>

namespace unit
{

    class GLAABBTest : public QObject
    {
        Q_OBJECT

    public:
        explicit GLAABBTest(QObject *parent = 0);

    private Q_SLOTS:
        void initTestCase();
        void cleanupTestCase();

        void testConstructFromPoints();
        void testConstructFromPoints_data();

        void testContainsByPoint();
        void testContainsByPoint_data();

        void testContainsByAABB();
        void testContainsByAABB_data();

        void testIntersects();
        void testIntersects_data();

        void testCut();
        void testCut_data();

        void testJoin();
        void testJoin_data();
    };

} // namespace unit //

#endif // TST_GLAABBTEST_H //
