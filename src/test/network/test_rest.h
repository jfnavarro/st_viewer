/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef TEST_REST_H
#define TEST_REST_H

#include <QObject>

namespace unit
{

class RestTest : public QObject
{
    Q_OBJECT

public:
    explicit RestTest(QObject* parent = 0);

private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();
};

} // namespace unit //

#endif /* // TEST_REST_H */
