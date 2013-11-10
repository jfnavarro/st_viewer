/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef TST_OBJECTPARSERTEST_H
#define TST_OBJECTPARSERTEST_H

#include <QObject>

namespace unit
{

class ObjectParserTest : public QObject
{
    Q_OBJECT

public:
    explicit ObjectParserTest(QObject *parent = 0);

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testParseObject();
    void testParseObject_data();
};

} // namespace unit //

#endif // TST_OBJECTPARSERTEST_H
