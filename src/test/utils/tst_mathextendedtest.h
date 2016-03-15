#ifndef TST_MATHEXTENDEDTEST_H
#define TST_MATHEXTENDEDTEST_H

#include <QObject>

namespace unit
{

class MathExtendedTest : public QObject
{
    Q_OBJECT

public:
    explicit MathExtendedTest(QObject* parent = 0);

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testFloatMod();
    void testFloatMod_data();

    void testClamp();
    void testClamp_data();
};

} // namespace unit //

#endif // TST_MATHEXTENDEDTEST_H
