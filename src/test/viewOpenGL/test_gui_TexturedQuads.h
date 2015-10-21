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
class TexturedQuadsTest : public QObject
{
    Q_OBJECT

public:
    explicit TexturedQuadsTest(QObject* parent = 0);

private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();

    void test_equals_different_quads();
    void test_equality_uvcoords();

    void test_quadTextureCoords();

    void test_defaultTextureCoords();

    void test_drawingMissingTextureTexturedQuad();

    void test_drawingTexturedQuad();
};
}

#endif
