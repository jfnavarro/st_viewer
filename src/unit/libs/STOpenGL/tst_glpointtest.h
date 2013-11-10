/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef TST_GLPOINTTEST_H
#define TST_GLPOINTTEST_H

#include <QObject>

namespace unit
{

class GLPointTest : public QObject
{
    Q_OBJECT

public:
    explicit GLPointTest(QObject *parent = 0);

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
};

} // namespace unit //

#endif // TST_GLPOINTTEST_H //
