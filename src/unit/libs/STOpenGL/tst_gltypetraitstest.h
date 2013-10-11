/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef TST_GLTYPETRAITSTEST_H
#define TST_GLTYPETRAITSTEST_H

#include <QObject>

namespace unit
{

    class GLTypeTraitsTest : public QObject
    {
        Q_OBJECT

    public:
        explicit GLTypeTraitsTest(QObject *parent = 0);

    private Q_SLOTS:
        void initTestCase();
        void cleanupTestCase();

        void testTypeTraitCompileTypeEnum();
        void testTypeTraitCompileMinMax();

        void testTypeEnumTraitTypeSize();
        void testTypeEnumInfoTypeSize();

        void testGeomEnumTraitVertexCount();
        void testGeomEnumInfoVertexCount();
    };

} // namespace unit //

#endif // TST_GLTYPETRAITSTEST_H //
