/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QDebug>

#include <QtTest/QTest>

#include <qgl.h>

#include <GLTypeTraits.h>
//#include <GLCommon.h>
//Q_DECLARE_METATYPE(GL::GLpoint)

#include "tst_gltypetraitstest.h"

namespace unit
{

    GLTypeTraitsTest::GLTypeTraitsTest(QObject *parent) : QObject(parent) { }

    void GLTypeTraitsTest::initTestCase()
	{
		QVERIFY2( true, "Empty");
	}
    void GLTypeTraitsTest::cleanupTestCase()
	{
		QVERIFY2( true, "Empty");
	}

    void GLTypeTraitsTest::testTypeTraitCompileTypeEnum()
    {
        QCOMPARE( GLenum(GL_BYTE), GL::GLTypeTrait<GLbyte>::type_enum );
        QCOMPARE( GLenum(GL_UNSIGNED_BYTE), GL::GLTypeTrait<GLubyte>::type_enum );
        QCOMPARE( GLenum(GL_SHORT), GL::GLTypeTrait<GLshort>::type_enum );
        QCOMPARE( GLenum(GL_UNSIGNED_SHORT), GL::GLTypeTrait<GLushort>::type_enum );
        QCOMPARE( GLenum(GL_INT), GL::GLTypeTrait<GLint>::type_enum );
        QCOMPARE( GLenum(GL_UNSIGNED_INT), GL::GLTypeTrait<GLuint>::type_enum );
        QCOMPARE( GLenum(GL_FLOAT), GL::GLTypeTrait<GLfloat>::type_enum );
        QCOMPARE( GLenum(GL_DOUBLE), GL::GLTypeTrait<GLdouble>::type_enum );
    }

    void GLTypeTraitsTest::testTypeTraitCompileMinMax()
    {
        // min value
        QCOMPARE( std::numeric_limits<GLbyte>::min(), GL::GLTypeTrait<GLbyte>::min_value );
        QCOMPARE( std::numeric_limits<GLubyte>::min(), GL::GLTypeTrait<GLubyte>::min_value );
        QCOMPARE( std::numeric_limits<GLshort>::min(), GL::GLTypeTrait<GLshort>::min_value );
        QCOMPARE( std::numeric_limits<GLushort>::min(), GL::GLTypeTrait<GLushort>::min_value );
        QCOMPARE( std::numeric_limits<GLint>::min(), GL::GLTypeTrait<GLint>::min_value );
        QCOMPARE( std::numeric_limits<GLuint>::min(), GL::GLTypeTrait<GLuint>::min_value );
        QCOMPARE( std::numeric_limits<GLfloat>::min(), GL::GLTypeTrait<GLfloat>::min_value );
        QCOMPARE( std::numeric_limits<GLdouble>::min(), GL::GLTypeTrait<GLdouble>::min_value );

        // max value
        QCOMPARE( std::numeric_limits<GLbyte>::max(), GL::GLTypeTrait<GLbyte>::max_value );
        QCOMPARE( std::numeric_limits<GLubyte>::max(), GL::GLTypeTrait<GLubyte>::max_value );
        QCOMPARE( std::numeric_limits<GLshort>::max(), GL::GLTypeTrait<GLshort>::max_value );
        QCOMPARE( std::numeric_limits<GLushort>::max(), GL::GLTypeTrait<GLushort>::max_value );
        QCOMPARE( std::numeric_limits<GLint>::max(), GL::GLTypeTrait<GLint>::max_value );
        QCOMPARE( std::numeric_limits<GLuint>::max(), GL::GLTypeTrait<GLuint>::max_value );
        QCOMPARE( std::numeric_limits<GLfloat>::max(), GL::GLTypeTrait<GLfloat>::max_value );
        QCOMPARE( std::numeric_limits<GLdouble>::max(), GL::GLTypeTrait<GLdouble>::max_value );
    }

    void GLTypeTraitsTest::testTypeEnumTraitTypeSize()
    {
        QCOMPARE( int(sizeof(GLbyte)), GL::GLTypeEnumTrait<GL_BYTE>::type_size );
        QCOMPARE( int(sizeof(GLubyte)), GL::GLTypeEnumTrait<GL_UNSIGNED_BYTE>::type_size );
        QCOMPARE( int(sizeof(GLshort)), GL::GLTypeEnumTrait<GL_SHORT>::type_size );
        QCOMPARE( int(sizeof(GLushort)), GL::GLTypeEnumTrait<GL_UNSIGNED_SHORT>::type_size );
        QCOMPARE( int(sizeof(GLint)), GL::GLTypeEnumTrait<GL_INT>::type_size );
        QCOMPARE( int(sizeof(GLuint)), GL::GLTypeEnumTrait<GL_UNSIGNED_INT>::type_size );
        QCOMPARE( int(sizeof(GLfloat)), GL::GLTypeEnumTrait<GL_FLOAT>::type_size );
        QCOMPARE( int(sizeof(GLdouble)), GL::GLTypeEnumTrait<GL_DOUBLE>::type_size );
    }
    void GLTypeTraitsTest::testTypeEnumInfoTypeSize()
    {
        QCOMPARE( int(sizeof(GLbyte)), GL::GLTypeEnumInfo::type_size(GL_BYTE) );
        QCOMPARE( int(sizeof(GLubyte)), GL::GLTypeEnumInfo::type_size(GL_UNSIGNED_BYTE) );
        QCOMPARE( int(sizeof(GLshort)), GL::GLTypeEnumInfo::type_size(GL_SHORT) );
        QCOMPARE( int(sizeof(GLushort)), GL::GLTypeEnumInfo::type_size(GL_UNSIGNED_SHORT) );
        QCOMPARE( int(sizeof(GLint)), GL::GLTypeEnumInfo::type_size(GL_INT) );
        QCOMPARE( int(sizeof(GLuint)), GL::GLTypeEnumInfo::type_size(GL_UNSIGNED_INT) );
        QCOMPARE( int(sizeof(GLfloat)), GL::GLTypeEnumInfo::type_size(GL_FLOAT) );
        QCOMPARE( int(sizeof(GLdouble)), GL::GLTypeEnumInfo::type_size(GL_DOUBLE) );
    }

    void GLTypeTraitsTest::testGeomEnumTraitVertexCount()
    {
        QCOMPARE( 1, GL::GLGeomEnumTrait<GL_POINTS>::vertex_count );
        QCOMPARE( 2, GL::GLGeomEnumTrait<GL_LINES>::vertex_count );
        QCOMPARE( 3, GL::GLGeomEnumTrait<GL_TRIANGLES>::vertex_count );
        QCOMPARE( 4, GL::GLGeomEnumTrait<GL_QUADS>::vertex_count );
    }
    void GLTypeTraitsTest::testGeomEnumInfoVertexCount()
    {
        QCOMPARE( 1, GL::GLGeomEnumInfo::vertex_count(GL_POINTS) );
        QCOMPARE( 2, GL::GLGeomEnumInfo::vertex_count(GL_LINES) );
        QCOMPARE( 3, GL::GLGeomEnumInfo::vertex_count(GL_TRIANGLES) );
        QCOMPARE( 4, GL::GLGeomEnumInfo::vertex_count(GL_QUADS) );
    }

} // namespace unit //
