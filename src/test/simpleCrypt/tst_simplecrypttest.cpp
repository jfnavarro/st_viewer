/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QtTest/QTest>
#include <QDebug>

#include "data/SimpleCrypt.h"

#include "tst_simplecrypttest.h"

namespace unit
{

const quint64 SimpleCryptTest::DEFAULT_TEST_KEY = Q_UINT64_C(0x16848f7d63c64889);
//const quint32 SimpleCryptTest::DEFAULT_TEST_COMPRESSION = static_cast<quint32>(SimpleCrypt::CompressionNever);
//const quint32 SimpleCryptTest::DEFAULT_TEST_PROTECTION  = static_cast<quint32>(SimpleCrypt::ProtectionChecksum);

SimpleCryptTest::SimpleCryptTest(QObject *parent) : QObject(parent) { }

void SimpleCryptTest::initTestCase()
{
    SimpleCrypt crypt;
    QVERIFY(!crypt.hasKey());

    crypt.setKey(DEFAULT_TEST_KEY);
    QVERIFY(crypt.hasKey());

    //crypt.setCompressionMode(SimpleCrypt::CompressionNever);
    //QCOMPARE( crypt.compressionMode(), SimpleCrypt::CompressionNever );

    //crypt.setIntegrityProtectionMode(SimpleCrypt::ProtectionChecksum);
    //QCOMPARE( crypt.integrityProtectionMode(), SimpleCrypt::ProtectionChecksum );
}

void SimpleCryptTest::cleanupTestCase()
{
    QVERIFY2(true, "Empty");
}

//void SimpleCryptTest::testEncodeData()
//{
//    QFETCH(QByteArray, input);
//    QFETCH(QByteArray, output);
//    QFETCH(bool, expected);

//    SimpleCrypt crypt(DEFAULT_TEST_KEY);
//    crypt.setCompressionMode(static_cast<SimpleCrypt::CompressionMode>(DEFAULT_TEST_COMPRESSION));
//    crypt.setIntegrityProtectionMode(static_cast<SimpleCrypt::IntegrityProtectionMode>(DEFAULT_TEST_PROTECTION));

//    qsrand(0); // ensure deterministic encryption during test
//    QCOMPARE( crypt.encryptToByteArray(input) == output, expected );
//}
//void SimpleCryptTest::testEncodeData_data()
//{
//    static const char input[4][32] = {
//        {0x8eu,0x0au,0x93u,0x8cu,0xfdu,0x3eu,0x5eu,0xdfu,0xefu,0x50u,0x9bu,0x7au,0x75u,0x71u,0x64u,0xeeu,0x75u,0x69u,0xf8u,0x91u,0xf8u,0x8fu,0x48u,0x7fu,0xf2u,0xb7u,0xfau,0x7au,0x93u,0xdfu,0xbdu,0x49u},
//        {0x04u,0x12u,0x0au,0x0cu,0x1fu,0x74u,0xfeu,0x23u,0x9au,0xd6u,0x0fu,0x32u,0x4fu,0x95u,0x6du,0x9bu,0x74u,0x71u,0x77u,0x0bu,0xccu,0x82u,0xb7u,0x17u,0xe6u,0xe9u,0x9fu,0xe5u,0x1fu,0x3au,0x46u,0xb8u},
//        {0x44u,0xb7u,0xb7u,0x90u,0x8cu,0x03u,0x8fu,0x43u,0x4bu,0x7au,0xdau,0x1du,0xd2u,0xdau,0xcbu,0x32u,0x1fu,0x8bu,0x73u,0x54u,0x59u,0x8cu,0xf5u,0x19u,0x91u,0x21u,0x25u,0xb4u,0x38u,0x9cu,0x81u,0xb9u},
//        {0x20u,0x63u,0x29u,0xecu,0x39u,0xf7u,0xf5u,0xa6u,0x94u,0xa8u,0x73u,0xd9u,0x59u,0x0fu,0xc3u,0x92u,0xd1u,0x2du,0xa5u,0xecu,0x2du,0xb5u,0x0fu,0x22u,0x92u,0x14u,0x95u,0xf3u,0xe8u,0x52u,0xa4u,0x90u}
//    };
//    static const char output[4][37] = {
//        {0x03u,0x02u,0xafu,0xdau,0x95u,0x78u,0x0fu,0x13u,0x1bu,0xf0u,0x47u,0x51u,0x48u,0xc4u,0xe9u,0xfdu,0x03u,0x60u,0x98u,0xb4u,0x9cu,0x8au,0x9eu,0xe9u,0xfcu,0x12u,0x14u,0x14u,0xadu,0x3cu,0xf6u,0x83u,0x7du,0xf8u,0xaeu,0x5bu,0xd4u},
//        {0x03u,0x02u,0xafu,0x0bu,0xd5u,0xb2u,0xddu,0x58u,0xd0u,0xd9u,0x24u,0x92u,0x77u,0x8eu,0x25u,0xa5u,0x13u,0x4au,0x56u,0x73u,0x2eu,0x39u,0x35u,0xcdu,0x42u,0x98u,0x93u,0x6cu,0xbdu,0x38u,0xacu,0xbcu,0xddu,0xd4u,0x67u,0x69u,0x17u},
//        {0x03u,0x02u,0xafu,0xcdu,0xfeu,0xd9u,0x13u,0x2bu,0x3fu,0xa5u,0x2fu,0xe8u,0x6du,0x45u,0x42u,0x17u,0x8eu,0x4au,0x19u,0x9au,0x6eu,0x12u,0xe4u,0x18u,0xc8u,0x87u,0x82u,0x3fu,0xe0u,0x12u,0x4eu,0xe4u,0xd4u,0xfau,0xefu,0x26u,0x59u},
//        {0x03u,0x02u,0xafu,0xfcu,0xebu,0xa8u,0xb6u,0x10u,0x78u,0x57u,0x29u,0x94u,0xf4u,0x03u,0xd6u,0x2au,0x77u,0x38u,0xbeu,0x35u,0x61u,0xd3u,0x83u,0xa9u,0xc1u,0xfau,0xc6u,0x81u,0x65u,0x94u,0xfdu,0xe7u,0x90u,0x6eu,0xb5u,0x59u,0x0fu}
//    };

//    QTest::addColumn<QByteArray>("input");
//    QTest::addColumn<QByteArray>("output");
//    QTest::addColumn<bool>("expected");

//    QTest::newRow("Binary0") << QByteArray::fromRawData(input[0], sizeof(input[0])) << QByteArray::fromRawData(output[0], sizeof(output[0])) << true;
//    QTest::newRow("Binary1") << QByteArray::fromRawData(input[1], sizeof(input[1])) << QByteArray::fromRawData(output[1], sizeof(output[1])) << true;
//    QTest::newRow("Binary2") << QByteArray::fromRawData(input[2], sizeof(input[2])) << QByteArray::fromRawData(output[2], sizeof(output[2])) << true;
//    QTest::newRow("Binary3") << QByteArray::fromRawData(input[3], sizeof(input[3])) << QByteArray::fromRawData(output[3], sizeof(output[3])) << true;
//}

//void SimpleCryptTest::testDecodeData()
//{
//    QFETCH(QByteArray, input);
//    QFETCH(QByteArray, output);
//    QFETCH(bool, expected);

//    SimpleCrypt crypt(DEFAULT_TEST_KEY);
//    crypt.setCompressionMode(static_cast<SimpleCrypt::CompressionMode>(DEFAULT_TEST_COMPRESSION));
//    crypt.setIntegrityProtectionMode(static_cast<SimpleCrypt::IntegrityProtectionMode>(DEFAULT_TEST_PROTECTION));

//    qsrand(0); // ensure deterministic encryption during test
//    QCOMPARE( crypt.decryptToByteArray(input) == output, expected );
//}
//void SimpleCryptTest::testDecodeData_data()
//{
//    static const char input[4][37] = {
//        {0x03u,0x02u,0x5fu,0xf6u,0xffu,0xe1u,0xddu,0x55u,0x07u,0xb3u,0xf1u,0x2bu,0xcbu,0xb5u,0xc5u,0x27u,0x3du,0xccu,0x37u,0xccu,0xfbu,0x1bu,0xb8u,0xeau,0x00u,0x2du,0xd2u,0xc6u,0xcbu,0xe0u,0x9au,0xdfu,0x23u,0x9au,0x06u,0xd2u,0x8eu},
//        {0x03u,0x02u,0x55u,0xadu,0x00u,0x6bu,0x2eu,0x6cu,0x8du,0x06u,0x5au,0xf7u,0xbbu,0xf0u,0x2fu,0x31u,0xbdu,0x09u,0x0au,0x6au,0xeeu,0xa5u,0x7au,0x7fu,0xebu,0x4fu,0xffu,0x83u,0xe2u,0x83u,0x2au,0x59u,0x1bu,0xfau,0x47u,0x88u,0x41u},
//        {0x03u,0x02u,0x55u,0x47u,0x66u,0x1au,0xadu,0xaeu,0xf8u,0x95u,0x44u,0xe6u,0x05u,0x21u,0xd8u,0xb0u,0xa8u,0xedu,0x97u,0x92u,0x12u,0x27u,0x9cu,0xd8u,0x16u,0x52u,0x0au,0x91u,0x73u,0x11u,0x85u,0xcbu,0x72u,0xdau,0x86u,0xb9u,0x74u},
//        {0x03u,0x02u,0x56u,0x8eu,0x8au,0xf1u,0xc2u,0xb7u,0x0fu,0x8cu,0x79u,0xa8u,0xd2u,0x97u,0x32u,0xe3u,0x0bu,0xecu,0xd2u,0xf3u,0xe2u,0xedu,0xffu,0x31u,0xd0u,0x8eu,0x72u,0xdeu,0xa2u,0x9cu,0x38u,0x4fu,0x46u,0xf8u,0x0bu,0xb0u,0xaau}
//    };
//    static const char output[4][32] = {
//        {0x7du,0x41u,0x07u,0xd6u,0xa2u,0xcbu,0x92u,0x26u,0x1du,0x0du,0x6du,0x9eu,0xe7u,0x72u,0xb3u,0xf1u,0x83u,0xdeu,0xddu,0x6eu,0x3bu,0x76u,0x5cu,0xcbu,0x48u,0x07u,0xcau,0x78u,0xafu,0x15u,0x9cu,0x9au},
//        {0x08u,0x38u,0xcdu,0x65u,0x9du,0xd5u,0xe5u,0x8au,0x28u,0xa2u,0x91u,0x08u,0xa2u,0x8au,0x28u,0x42u,0x28u,0xa2u,0x8au,0x10u,0xb2u,0x39u,0x34u,0xa7u,0x02u,0xd4u,0xfcu,0xc6u,0xf7u,0x34u,0x87u,0x0fu},
//        {0x1fu,0xcau,0x8cu,0xd2u,0x7bu,0x58u,0xeau,0x25u,0x47u,0x84u,0xe7u,0x9cu,0x53u,0xf3u,0x4du,0x46u,0x56u,0xc6u,0xcbu,0x4au,0x52u,0xd1u,0xd3u,0x24u,0x01u,0xe9u,0xc1u,0x3du,0xbeu,0xd5u,0x77u,0x0bu},
//        {0x18u,0x4eu,0xfau,0x3cu,0x95u,0x7cu,0x99u,0xbcu,0x26u,0xd8u,0x5eu,0x6cu,0xf1u,0xb7u,0x69u,0xd7u,0x6cu,0x6fu,0x41u,0x65u,0x48u,0x75u,0xe4u,0xbau,0x5du,0xd9u,0xf8u,0x8du,0xa8u,0x7au,0xf3u,0xdcu}
//    };

//    QTest::addColumn<QByteArray>("input");
//    QTest::addColumn<QByteArray>("output");
//    QTest::addColumn<bool>("expected");

//    QTest::newRow("Binary0") << QByteArray::fromRawData(input[0], sizeof(input[0])) << QByteArray::fromRawData(output[0], sizeof(output[0])) << true;
//    QTest::newRow("Binary1") << QByteArray::fromRawData(input[1], sizeof(input[1])) << QByteArray::fromRawData(output[1], sizeof(output[1])) << true;
//    QTest::newRow("Binary2") << QByteArray::fromRawData(input[2], sizeof(input[2])) << QByteArray::fromRawData(output[2], sizeof(output[2])) << true;
//    QTest::newRow("Binary3") << QByteArray::fromRawData(input[3], sizeof(input[3])) << QByteArray::fromRawData(output[3], sizeof(output[3])) << true;
//}

//void SimpleCryptTest::testEncodeString()
//{
//    QFETCH(QString, input);
//    QFETCH(QString, output);
//    QFETCH(int, expected);

//    SimpleCrypt crypt(DEFAULT_TEST_KEY);
//    crypt.setCompressionMode(static_cast<SimpleCrypt::CompressionMode>(DEFAULT_TEST_COMPRESSION));
//    crypt.setIntegrityProtectionMode(static_cast<SimpleCrypt::IntegrityProtectionMode>(DEFAULT_TEST_PROTECTION));

//    qsrand(0); // ensure deterministic encryption during test
//    QCOMPARE( QString::compare(crypt.encryptToString(input), output), expected );
//}
//void SimpleCryptTest::testEncodeString_data()
//{
//    static const QString input[] = {
//        ("Hack the planet! Hack the planet!"),
//        ("Mess with the best, die like the rest."),
//        ("Remember, hacking is more than just a crime. It's a survival trait."),
//        ("We have just gotten a wake-up call from the Nintendo Generation.")
//    };
//    static const QString output[] = { // encoded using base64, encrypted with default key and 0 random seed
//        ("AwKvMRU+Is4hF+rKaSonxCFZtYluLRj2EWzF+VdRDPMbbIumFFY="),
//        ("AwKvcm9BWaVSZJq7CQJfpEg7krgbCwKhBXeXulxTR6NCdImpCklGrFs5ng=="),
//        ("AwKvDoW0rE6v1D8Spum0U7bDIQCorPEX4NYyFaGn+gHtmn0Vua+hWv6JIAu/taVP5dMTL87eg23JrFBq2tDbNd3rFiyLgYgp"),
//        ("AwKv4mBUTOMPeIeqTEVNsUF3mb4MGwPiRjGYpwAIELJDJYynAA8esVM30fQSBRD6XgbmwHJ0Z4xnUZ+yGhwT/Q1ylLJa")
//    };

//    QTest::addColumn<QString>("input");
//    QTest::addColumn<QString>("output");
//    QTest::addColumn<int>("expected");

//    QTest::newRow("Quote0") << input[0] << output[0] << 0;
//    QTest::newRow("Quote1") << input[1] << output[1] << 0;
//    QTest::newRow("Quote2") << input[2] << output[2] << 0;
//    QTest::newRow("Quote3") << input[3] << output[3] << 0;
//}

//void SimpleCryptTest::testDecodeString()
//{
//    QFETCH(QString, input);
//    QFETCH(QString, output);
//    QFETCH(int, expected);

//    SimpleCrypt crypt(DEFAULT_TEST_KEY);
//    crypt.setCompressionMode(static_cast<SimpleCrypt::CompressionMode>(DEFAULT_TEST_COMPRESSION));
//    crypt.setIntegrityProtectionMode(static_cast<SimpleCrypt::IntegrityProtectionMode>(DEFAULT_TEST_PROTECTION));

//    qsrand(0); // ensure deterministic encryption during test
//    QCOMPARE( QString::compare(crypt.decryptToString(input), output), expected );
//}
//void SimpleCryptTest::testDecodeString_data()
//{
//    static const QString input[] = { // encoded using base64, encrypted with default key and 0 random seed
//        ("AwKv7wgqOc8/QaGHJmV9nmkav9d8cGDQdDvd4AYSDu8fKc3gBhEDrEU8wqoYEwukTDvF4xpZa4t/GrLaVRYN7RtqjLBaGSrAMwXV8kZOH7B6A6qFMTM9wWs="),
//        ("AwKvErCekTCUwCwIqKSrQPvNCTje088tzPoaIcfQxS+LzSgBoLauD6voCW2LpaoLr+0MIcf05Qvoi2cB"),
//        ("AwKvjgQme4NoDOiAByoTvFlvkLEFEx2tCUuqh2FyY4NzRbiYNzchyyFE4w=="),
//        ("AwKvKefdxSvH/VRryMfWddG+WGWDi5h4i71AYMPTy2Sq3SQNpaOtR+OYcFbx9e0P7pZrA7G3qU6k2yMeva3+"),
//    };
//    static const QString output[] = {
//        ("Anything else, mom? You want me to mow the lawn? Oops! I forgot, New York, No grass."),
//        ("Mr. Belford? My name is the Plague. Uh, Mr. The Plague."),
//        ("A worm AND a virus? The plot thickens."),
//        ("Yeah, well, you know these Japanese management techniques."),
//    };

//    QTest::addColumn<QString>("input");
//    QTest::addColumn<QString>("output");
//    QTest::addColumn<int>("expected");

//    QTest::newRow("Quote0") << input[0] << output[0] << 0;
//    QTest::newRow("Quote1") << input[1] << output[1] << 0;
//    QTest::newRow("Quote2") << input[2] << output[2] << 0;
//    QTest::newRow("Quote3") << input[3] << output[3] << 0;
//}

} // namespace unit //
