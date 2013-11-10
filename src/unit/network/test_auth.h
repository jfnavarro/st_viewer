/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef TEST_AUTH_H
#define TEST_AUTH_H

#include <QObject>


namespace unit
{

class AuthTest : public QObject
{
    Q_OBJECT

public:

    explicit AuthTest(QObject *parent = 0);

private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();

};

} // namespace unit //

#endif	/* // TEST_AUTH_H */
