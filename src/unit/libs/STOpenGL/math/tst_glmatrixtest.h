/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef TST_GLMATRIXTEST_H
#define TST_GLMATRIXTEST_H

#include <QObject>

namespace unit
{

class GLMatrixTest : public QObject
{
    Q_OBJECT

public:
    explicit GLMatrixTest(QObject *parent = 0);

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCompareEqual();
    void testCompareEqual_data();
};

} // namespace unit //

#endif // TST_GLMATRIXTEST_H //
