#ifndef TST_WIDGETS_H
#define TST_WIDGETS_H

#include <QObject>

namespace Ui
{
class MainWindow;
} // namespace Ui //

namespace unit
{

class WidgetsTest : public QObject
{
    Q_OBJECT

public:
    explicit WidgetsTest(QObject* parent = 0);

private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();

    // This test exists because QTest reports test objects with no tests as failures.
    void testDoesNothing();
};

} // namespace unit //

#endif // TST_WIDGETS_H
