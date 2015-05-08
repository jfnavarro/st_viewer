/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef TESTSUITE_H
#define TESTSUITE_H

#include <QObject>

namespace unit
{

class TestSuite;
class TestHandle
{

public:

    TestHandle();
    virtual ~TestHandle();

    TestHandle(TestSuite *suite, const QString &name);

    TestHandle dependsOn(const QString &name);

private:
    TestSuite *m_suite;
    const QString m_name;
};

class TestSuite
{

public:

    TestSuite();
    virtual ~TestSuite();

    TestHandle addTest(QObject *test, const QString &name);

    int exec();

private:

    friend class TestHandle;

    void setDependencie(const QString &parent, const QString &child);

    QObject m_root;
};

} // namespace unit //

#endif // TESTSUITE_H
