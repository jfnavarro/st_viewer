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

    Q_PROPERTY(unsigned x1 READ x1 WRITE x1)
    Q_PROPERTY(unsigned x2 READ x2 WRITE x2)
    Q_PROPERTY(unsigned x1_total READ x1Total WRITE x1Total)
    Q_PROPERTY(unsigned x2_total READ x2Total WRITE x2Total)
    Q_PROPERTY(unsigned x1_border READ x1Border WRITE x1Border)
    Q_PROPERTY(unsigned x2_border READ x2Border WRITE x2Border)
    Q_PROPERTY(unsigned y1 READ y1 WRITE y1)
    Q_PROPERTY(unsigned y2 READ y2 WRITE y2)
    Q_PROPERTY(unsigned y1_total READ y1Total WRITE y1Total)
    Q_PROPERTY(unsigned y2_total READ y2Total WRITE y2Total)
    Q_PROPERTY(unsigned y1_border READ y1Border WRITE y1Border)
    Q_PROPERTY(unsigned y2_border READ y2Border WRITE y2Border)
    Q_PROPERTY(unsigned barcodes READ spots WRITE spots)
    Q_PROPERTY(QString id READ id WRITE id)
    Q_PROPERTY(QString name READ name WRITE name)
    Q_PROPERTY(QString created_at READ created WRITE created)
    Q_PROPERTY(QString last_modified READ lastModified WRITE lastModified)

public:
    explicit ChipDTO(QObject *parent = 0)
        : QObject(parent)
    {
    }
    ~ChipDTO() {}

    // binding
    void id(const QString &id) { m_chip.id(id); }
    void spots(unsigned spots) { m_chip.spots(spots); }
    void name(const QString &name) { m_chip.name(name); }
    void x1(unsigned x1) { m_chip.x1(x1); }
    void x2(unsigned x2) { m_chip.x2(x2); }
    void x1Total(unsigned x1Total) { m_chip.x1Total(x1Total); }
    void x2Total(unsigned x2Total) { m_chip.x2Total(x2Total); }
    void x1Border(unsigned x1Border) { m_chip.x1Border(x1Border); }
    void x2Border(unsigned x2Border) { m_chip.x2Border(x2Border); }
    void y1(unsigned y1) { m_chip.y1(y1); }
    void y2(unsigned y2) { m_chip.y2(y2); }
    void y1Total(unsigned y1Total) { m_chip.y1Total(y1Total); }
    void y2Total(unsigned y2Total) { m_chip.y2Total(y2Total); }
    void y1Border(unsigned y1Border) { m_chip.y1Border(y1Border); }
    void y2Border(unsigned y2Border) { m_chip.y2Border(y2Border); }
    void created(const QString &created) { m_chip.created(created); }
    void lastModified(const QString &lastModified) { m_chip.lastModified(lastModified); }

    // read
    const QString id() { return m_chip.id(); }
    unsigned spots() { return m_chip.spots(); }
    const QString name() { return m_chip.name(); }
    unsigned x1() { return m_chip.x1(); }
    unsigned x2() { return m_chip.x2(); }
    unsigned x1Total() { return m_chip.x1Total(); }
    unsigned x2Total() { return m_chip.x2Total(); }
    unsigned x1Border() { return m_chip.x1Border(); }
    unsigned x2Border() { return m_chip.x2Border(); }
    unsigned y1() { return m_chip.y1(); }
    unsigned y2() { return m_chip.y2(); }
    unsigned y1Total() { return m_chip.y1Total(); }
    unsigned y2Total() { return m_chip.y2Total(); }
    unsigned y1Border() { return m_chip.y1Border(); }
    unsigned y2Border() { return m_chip.y2Border(); }
    const QString created() const { return m_chip.created(); }
    const QString lastModified() const { return m_chip.lastModified(); }

    // get parsed data model
    const Chip &chip() const { return m_chip; }
    Chip &chip() { return m_chip; }

private:
    Chip m_chip;
};

#endif // CHIPDTO_H //
