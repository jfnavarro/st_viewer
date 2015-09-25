/*
Copyright (C) 2015  Spatial Transcriptomics AB,
read LICENSE for licensing terms.
Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>
*/

#ifndef TST_ASSERTOPENGL_H
#define TST_ASSERTOPENGL_H

#include <QObject>

namespace unit
{

// Tests associated with the OpenGLAssert.h file. We cannot test an actual assert, so we test
// the components used by the OpenGL assert macro.
class OpenGLAssertTest : public QObject
{
    Q_OBJECT

public:
    explicit OpenGLAssertTest(QObject* parent = 0);

private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();

    void test_checkOpenGLErrorWithNoErrors();
    void test_checkOpenGLErrorDetectsError();
};
}

#endif
