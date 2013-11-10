/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef TST_GLMATHTEST_H
#define TST_GLMATHTEST_H

#include <QObject>

namespace unit
{

class GLMathTest : public QObject
{
    Q_OBJECT

public:
    explicit GLMathTest(QObject *parent = 0);

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testRangeIncl();
    void testRangeIncl_data();

    void testRangeExcl();
    void testRangeExcl_data();
};

} // namespace //

#endif // TST_GLMATHTEST_H //
