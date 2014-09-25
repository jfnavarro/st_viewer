/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QtTest/QTest>

#include "data/DataStore.h"

#include "tst_datastoretest.h"

namespace unit
{

DataStoreTest::DataStoreTest(QObject *parent) : QObject(parent) { }

void DataStoreTest::initTestCase()
{
    DataStore *dataStore = new DataStore;
    QVERIFY(dataStore != nullptr);
}

void DataStoreTest::cleanupTestCase()
{
    DataStore *dataStore = new DataStore;
    delete dataStore;
    dataStore = nullptr;
    //TODO this is a dumb test
    QVERIFY(dataStore == nullptr);
}

void DataStoreTest::testCreateFile()
{
    QFETCH(QString, name);
    QFETCH(uint, options);
    QFETCH(bool, expected);

    DataStore dataStore;

    // create and verify
    dataStore.createResource(name, static_cast<DataStore::Option>(options));
    auto file = dataStore.accessResource(name, static_cast<DataStore::Option>(options));
    QVERIFY(file != nullptr);

    // test that file is registered in data store correctly
    QCOMPARE(dataStore.hasResource(name), expected);

    // test that file is created (on disk) correctly
    QVERIFY(file->open(QIODevice::WriteOnly));
    QVERIFY(file->isOpen());
    QVERIFY(QFile::exists(name));
    file->close();

    // test that files is removed
    bool removed = QFile::remove(name);
    QVERIFY(removed);
}

void DataStoreTest::testCreateFile_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<uint>("options");
    QTest::addColumn<bool>("expected");

    QTest::newRow("normal") << QString("somefilename")
                            << static_cast<uint>(DataStore::Empty) << true;
    QTest::newRow("temporary") << QString("someotherfilename")
                               << static_cast<uint>(DataStore::Empty) << true;
}

} // namespace unit //
