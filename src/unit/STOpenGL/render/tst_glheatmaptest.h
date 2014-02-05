/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef TST_GLHEATMAP_H
#define TST_GLHEATMAP_H

#include <QObject>

namespace unit
{

class GLHeatMapTest : public QObject
{
    Q_OBJECT

public:
    explicit GLHeatMapTest(QObject *parent = 0);

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testHeatMap();
    void testHeatMap_data();
};

} // namespace unit //

#endif // TST_GLHEATMAP_H //
