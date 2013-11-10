/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef TST_SIMPLECRYPTDEVICETEST_H
#define TST_SIMPLECRYPTDEVICETEST_H

#include <QObject>

namespace unit
{

class SimpleCryptDeviceTest : public QObject
{
    Q_OBJECT

public:
    explicit SimpleCryptDeviceTest(QObject *parent = 0);

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testEncodeData();
    void testEncodeData_data();

    void testDecodeData();
    void testDecodeData_data();

    void testLargeFile();
    void testLargeFile_data();
};

} // namespace unit //

#endif // TST_SIMPLECRYPTDEVICETEST_H //
