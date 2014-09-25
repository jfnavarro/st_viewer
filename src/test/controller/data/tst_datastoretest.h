/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef TST_DATASTORETEST_H
#define TST_DATASTORETEST_H

#include <QObject>

namespace unit
{

class DataStoreTest : public QObject
{
    Q_OBJECT

public:

    explicit DataStoreTest(QObject *parent = 0);

private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();

    //these tests do more than create
    void testCreateFile();
    void testCreateFile_data();

    //TODO add test for the clear functions

    //TODO add test for the last_modified functions
};

} // namespace unit //

#endif // TST_DATASTORETEST_H
