/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef CHIPDTO_H
#define CHIPDTO_H

#include <QObject>
#include <QString>

#include "dataModel/Chip.h"

// DTOs (Data transform objects) provides the bridge between the client
// specific data model objects and the server data. This is accomplished by
// defining dynamic properties that enable automated serialization and
// deserialization of server data.
// ChipDTO defines the parsing object for the underlying Chip data object.

// TODO move definitions to CPP and/or consider removing DTOs
class ChipDTO : public QObject
{

    Q_OBJECT

    Q_PROPERTY(int x1 READ x1 WRITE x1)
    Q_PROPERTY(int x2 READ x2 WRITE x2)
    Q_PROPERTY(int x1_total READ x1Total WRITE x1Total)
    Q_PROPERTY(int x2_total READ x2Total WRITE x2Total)
    Q_PROPERTY(int x1_border READ x1Border WRITE x1Border)
    Q_PROPERTY(int x2_border READ x2Border WRITE x2Border)
    Q_PROPERTY(int y1 READ y1 WRITE y1)
    Q_PROPERTY(int y2 READ y2 WRITE y2)
    Q_PROPERTY(int y1_total READ y1Total WRITE y1Total)
    Q_PROPERTY(int y2_total READ y2Total WRITE y2Total)
    Q_PROPERTY(int y1_border READ y1Border WRITE y1Border)
    Q_PROPERTY(int y2_border READ y2Border WRITE y2Border)
    Q_PROPERTY(int barcodes READ barcodes WRITE barcodes)
    Q_PROPERTY(QString id READ id WRITE id)
    Q_PROPERTY(QString name READ name WRITE name)
    Q_PROPERTY(QString created_at READ created WRITE created)
    Q_PROPERTY(QString last_modified READ lastModified WRITE lastModified)

public:
    explicit ChipDTO(QObject* parent = 0)
        : QObject(parent)
    {
    }
    ~ChipDTO() {}

    // binding
    void id(const QString& id) { m_chip.id(id); }
    void barcodes(int barcodes) { m_chip.barcodes(barcodes); }
    void name(const QString& name) { m_chip.name(name); }
    void x1(int x1) { m_chip.x1(x1); }
    void x2(int x2) { m_chip.x2(x2); }
    void x1Total(int x1Total) { m_chip.x1Total(x1Total); }
    void x2Total(int x2Total) { m_chip.x2Total(x2Total); }
    void x1Border(int x1Border) { m_chip.x1Border(x1Border); }
    void x2Border(int x2Border) { m_chip.x2Border(x2Border); }
    void y1(int y1) { m_chip.y1(y1); }
    void y2(int y2) { m_chip.y2(y2); }
    void y1Total(int y1Total) { m_chip.y1Total(y1Total); }
    void y2Total(int y2Total) { m_chip.y2Total(y2Total); }
    void y1Border(int y1Border) { m_chip.y1Border(y1Border); }
    void y2Border(int y2Border) { m_chip.y2Border(y2Border); }
    void created(const QString& created) { m_chip.created(created); }
    void lastModified(const QString& lastModified) { m_chip.lastModified(lastModified); }

    // read
    const QString id() { return m_chip.id(); }
    int barcodes() { return m_chip.barcodes(); }
    const QString name() { return m_chip.name(); }
    int x1() { return m_chip.x1(); }
    int x2() { return m_chip.x2(); }
    int x1Total() { return m_chip.x1Total(); }
    int x2Total() { return m_chip.x2Total(); }
    int x1Border() { return m_chip.x1Border(); }
    int x2Border() { return m_chip.x2Border(); }
    int y1() { return m_chip.y1(); }
    int y2() { return m_chip.y2(); }
    int y1Total() { return m_chip.y1Total(); }
    int y2Total() { return m_chip.y2Total(); }
    int y1Border() { return m_chip.y1Border(); }
    int y2Border() { return m_chip.y2Border(); }
    const QString created() const { return m_chip.created(); }
    const QString lastModified() const { return m_chip.lastModified(); }

    // get parsed data model
    const Chip& chip() const { return m_chip; }
    Chip& chip() { return m_chip; }

private:
    Chip m_chip;
};

#endif // CHIPDTO_H //
