/*
Copyright (C) 2015  Spatial Transcriptomics AB,
read LICENSE for licensing terms.
Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>
*/

#ifndef TEST_GUI_COLOREDLINESTEST_H
#define TEST_GUI_COLOREDLINESTEST_H

#include <QObject>

namespace unit
{

class ColoredLinesTest : public QObject
{
    Q_OBJECT

public:
    explicit ColoredLinesTest(QObject* parent = 0);

private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();

    void test_equality();
    void test_redVerticalsGreenHorizontals();
    void test_alphaBlendedLines();
    void test_thickLines();
    void test_accessingLines();
    void test_exceptionAccessingNonExistantLine();
};
}

#endif
