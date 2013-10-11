/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef TST_GLFLOATTEST_H
#define TST_GLFLOATTEST_H

#include <QObject>

namespace unit
{

    class GLFloatTest : public QObject
    {
        Q_OBJECT

    public:
        explicit GLFloatTest(QObject *parent = 0);

    private Q_SLOTS:
        void initTestCase();
        void cleanupTestCase();

        void testBitSet();
        void testBitSet_data();

        void testBitClear();
        void testBitClear_data();

        void testBitTest();
        void testBitTest_data();
    };

} // namespace //

#endif // TST_GLFLOATTEST_H //
