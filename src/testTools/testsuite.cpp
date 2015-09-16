/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QtTest/QTest>

#include "algorithm/breadthfirsttopdown.h"

#include "testsuite.h"

namespace unit
{

TestHandle::TestHandle(TestSuite* suite, const QString& name)
    : m_suite(suite)
    , m_name(name)
{
}

TestHandle TestHandle::dependsOn(const QString& name)
{
    // <name> must execute before <m_name>
    m_suite->setDependencie(name, m_name);
    return (*this);
}

TestHandle::TestHandle()
{
}

TestSuite::TestSuite()
{
}

TestHandle::~TestHandle()
{
}

TestSuite::~TestSuite()
{
}

TestHandle TestSuite::addTest(QObject* test, const QString& name)
{
    // assign name to object and add it without dependencies (to root)
    test->setObjectName(name);
    test->setParent(&m_root);

    return TestHandle(this, name);
}

int TestSuite::exec()
{
    QScopedPointer<Linearizer> linearizer(new BreadthFirstTopDown());

    QList<QObject*> list = linearizer->list(&m_root);
    list.removeFirst(); // pop dummy root object

    int exitCode = 0;
    foreach (QObject* object, list) {
        if (QTest::qExec(object)) {
            exitCode = -1;
        }
    }

    return exitCode;
}

void TestSuite::setDependencie(const QString& parent, const QString& child)
{
    QObject* parentObject = m_root.findChild<QObject*>(parent, Qt::FindChildrenRecursively);
    QObject* childObject = m_root.findChild<QObject*>(child, Qt::FindChildrenRecursively);

    // abort if either is null
    if (!parentObject || !childObject) {
        return;
    }

    childObject->setParent(parentObject);
}

} // namespace unit //
