/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

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

    explicit WidgetsTest(QObject *parent = 0);

private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();

};

} // namespace unit //

#endif // TST_WIDGETS_H
