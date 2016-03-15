#include <QtTest/QTest>
#include <QtCore/QTime>
#include "math/QuadTree.h"

Q_DECLARE_METATYPE(QList<QPointF>)

#include "tst_glquadtreetest.h"

namespace unit
{

GLQuadTreeTest::GLQuadTreeTest(QObject* parent)
    : QObject(parent)
{
}

void GLQuadTreeTest::initTestCase()
{
    QVERIFY2(true, "Empty");
}
void GLQuadTreeTest::cleanupTestCase()
{
    QVERIFY2(true, "Empty");
}

void GLQuadTreeTest::testInsert()
{
    QFETCH(PointList, points);
    QFETCH(int, buckets);
    QFETCH(bool, expected);

    TestQuadTree quadTree(QSizeF(8.0f, 8.0f));

    int size = points.size();
    for (int i = 0; i < size; ++i) {
        QVERIFY2(quadTree.insert(points[i], i), "Unable to insert point!");
    }

    QCOMPARE((quadTree.buckets() == buckets), expected);
}
void GLQuadTreeTest::testInsert_data()
{
    QTest::addColumn<PointList>("points");
    QTest::addColumn<int>("buckets");
    QTest::addColumn<bool>("expected");

    const QPointF p[] = {QPointF(1.0f, 1.0f),
                         QPointF(3.0f, 1.0f),
                         QPointF(5.0f, 1.0f),
                         QPointF(7.0f, 1.0f),
                         QPointF(1.0f, 3.0f),
                         QPointF(3.0f, 3.0f),
                         QPointF(5.0f, 3.0f),
                         QPointF(7.0f, 3.0f),
                         QPointF(1.0f, 5.0f),
                         QPointF(3.0f, 5.0f),
                         QPointF(5.0f, 5.0f),
                         QPointF(7.0f, 5.0f),
                         QPointF(1.0f, 7.0f),
                         QPointF(3.0f, 7.0f),
                         QPointF(5.0f, 7.0f),
                         QPointF(7.0f, 7.0f),
                         QPointF(0.5f, 0.5f),
                         QPointF(7.5f, 0.5f)};

    // NOTE the point list is constructed incrementally and a copy is made of the point list for
    // each row
    PointList points;
    QTest::newRow("empty") << (points) << 1 << true;
    QTest::newRow("simple0") << (points << p[0] << p[2] << p[8] << p[10]) << 1 << true;
    QTest::newRow("simple1") << (points << p[1] << p[4] << p[5]) << 5 << true;
    QTest::newRow("simple2") << (points << p[3] << p[6] << p[7]) << 5 << true;
    QTest::newRow("simple3") << (points << p[16]) << 9 << true;
    QTest::newRow("simple4") << (points << p[17]) << 13 << true;
}

} // namespace unit //
QTEST_MAIN(unit::GLQuadTreeTest)
#include "tst_glquadtreetest.moc"
