/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef TST_GLCOMMONTEST_H
#define TST_GLCOMMONTEST_H

#include <QObject>

namespace unit
{

class GLCommonTest : public QObject
{
    Q_OBJECT

public:
    explicit GLCommonTest(QObject *parent = 0);

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testFuzzyEqual();
    void testFuzzyEqual_data();

    void testFuzzyNotEqual();
    void testFuzzyNotEqual_data();

    void testLineFromPoint();
    void testLineFromPoint_data();
};

} // namespace unit //

#endif // TST_GLCOMMONTEST_H
