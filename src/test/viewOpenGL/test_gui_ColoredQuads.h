/*
Copyright (C) 2015  Spatial Transcriptomics AB,
read LICENSE for licensing terms.
Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>
*/

#ifndef TEST_GUI_COLOREDQUADSTEST_H
#define TEST_GUI_COLOREDQUADSTEST_H

#include <QObject>

namespace unit
{
class ColoredQuadsTest : public QObject
{
    Q_OBJECT

public:
    explicit ColoredQuadsTest(QObject* parent = 0);

private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();

    void test_OpenGLQuads_ctor();
    void test_equals();
    void test_notequals();
    void test_addQuad();
    void test_setQuad();
    void test_quadColor();
    void test_quadCount();
    void test_drawingQuad();
    void test_drawLinesOnQuad();
    void test_alphaBlending();
};
}

#endif
