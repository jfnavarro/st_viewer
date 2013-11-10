/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef TST_GLVECTORTEST_H
#define TST_GLVECTORTEST_H

#include <QObject>

namespace unit
{

class GLVectorTest : public QObject
{
    Q_OBJECT

public:
    explicit GLVectorTest(QObject *parent = 0);

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCompareEqual();
    void testCompareEqual_data();

    void testUnary();
    void testUnary_data();

    void testAddSub();
    void testAddSub_data();

    void testMultDiv();
    void testMultDiv_data();

    void testNormalLength();
    void testNormalLength_data();

    void testQuadrant();
    void testQuadrant_data();

    void testFromPoint();
    void testFromPoint_data();
};

} // namespace //

#endif // TST_GLVECTORTEST_H //
