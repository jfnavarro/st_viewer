/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef TST_GLCOLORTEST_H
#define TST_GLCOLORTEST_H

#include <QObject>

namespace unit
{

class GLColorTest : public QObject
{
    Q_OBJECT

public:
    explicit GLColorTest(QObject *parent = 0);

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCompressToRGB8Bit();
    void testCompressToRGB8Bit_data();

    void testDecompressFromRGB8Bit();
    void testDecompressFromRGB8Bit_data();

    void testCompressToRGB16Bit();
    void testCompressToRGB16Bit_data();

    void testDecompressFromRGB16Bit();
    void testDecompressFromRGB16Bit_data();
};

} // namespace //


#endif // TST_GLCOLORTEST_H //
