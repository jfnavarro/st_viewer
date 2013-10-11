/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef TST_SIMPLECRYPTTEST_H
#define TST_SIMPLECRYPTTEST_H

#include <QObject>

namespace unit
{

    class SimpleCryptTest : public QObject
    {
        Q_OBJECT

    public:
        explicit SimpleCryptTest(QObject *parent = 0);

    private:
        static const quint64 DEFAULT_TEST_KEY;
        static const quint32 DEFAULT_TEST_COMPRESSION;
        static const quint32 DEFAULT_TEST_PROTECTION;

    private Q_SLOTS:
        void initTestCase();
        void cleanupTestCase();

        //void testEncodeString();
        //void testEncodeString_data();

        //void testDecodeString();
        //void testDecodeString_data();

        //void testEncodeData();
        //void testEncodeData_data();

        //void testDecodeData();
        //void testDecodeData_data();
    };

} // namespace unit //

#endif // TST_SIMPLECRYPTTEST_H //
