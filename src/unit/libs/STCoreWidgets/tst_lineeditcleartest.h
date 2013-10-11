/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef TST_LINEEDITCLEARTEST_H
#define TST_LINEEDITCLEARTEST_H

#include <QObject>

namespace unit
{

    class LineEditClearTest : public QObject
    {
        Q_OBJECT

    public:
        explicit LineEditClearTest(QObject *parent = 0);

    private Q_SLOTS:
        void initTestCase();
        void cleanupTestCase();

		void testClear();
		void testClear_data();
    };

} // namespace unit //

#endif // TST_LINEEDITCLEARTEST_H
