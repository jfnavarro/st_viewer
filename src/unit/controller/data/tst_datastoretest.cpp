/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QtTest/QTest>

#include "controller/data/DataStore.h"

#include "tst_datastoretest.h"

namespace unit
{

    DataStoreTest::DataStoreTest(QObject *parent) : QObject(parent) { }

    void DataStoreTest::initTestCase()
    {
        DataStore *dataStore = DataStore::getInstance();
        QVERIFY( dataStore != 0 );
    }

    void DataStoreTest::cleanupTestCase()
    {
        DataStore *dataStore = DataStore::getInstance(true);
        QVERIFY( dataStore == 0 );
    }

    void DataStoreTest::testCreateFile()
    {
        QFETCH(QString, name);
		QFETCH(uint, options);
		QFETCH(bool, expected);

        DataStore *dataStore = DataStore::getInstance();
		dataStore->init();

		// create and verify
        QIODevice *file = dataStore->accessResource(name, static_cast<ResourceStore::Option>(options));
		QVERIFY( file != 0 );

		// test that file is registered in data store correctly
        QCOMPARE( dataStore->hasResource(name), expected );

		// test that file is created (on disk) correctly
		QVERIFY( file->open(QIODevice::WriteOnly) );
        QVERIFY( file->isOpen() );
        QVERIFY( QFile::exists(name) );
        file->close();

		// test that files is removed
        bool removed = QFile::remove(name);
		QVERIFY( removed );
	}

    void DataStoreTest::testCreateFile_data()
    {
        QTest::addColumn<QString>("name");
        QTest::addColumn<uint>("options");
        QTest::addColumn<bool>("expected");

		QTest::newRow("normal") << QString("somefilename") << static_cast<uint>(DataStore::Empty) << true;
		QTest::newRow("temporary") << QString("someotherfilename") << static_cast<uint>(DataStore::Empty) << true;
    }

} // namespace unit //
