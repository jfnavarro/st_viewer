/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef CHIPDTO_H
#define CHIPDTO_H

#include <QObject>
#include <QString>

#include "model/Chip.h"

// DTOs (Data transform objects) provides the bridge between the client
// specific data model objects and the server data. This is accomplished by
// defining dynamic properties that enable automated serialization and
// deserialization of server data.

// ChipDTO defines the parsing object for the underlying Chip data object.
// Mapping Notes:
//     1:1 mapping. No conversions.
class ChipDTO : public QObject
{
    
public:
    
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

public:
    
    explicit ChipDTO(QObject* parent = 0);
    ChipDTO(const Chip& chip, QObject* parent = 0);
    virtual ~ChipDTO();

    // binding
    inline const QString id() const { return m_chip.id(); }
    inline int barcodes() const { return m_chip.barcodes(); }
    inline const QString name() const { return m_chip.name(); }
    inline int x1() const { return m_chip.x1(); }
    inline int x2() const { return m_chip.x2(); }
    inline int x1Total() const { return m_chip.x1Total(); }
    inline int x2Total() const { return m_chip.x2Total(); }
    inline int x1Border() const { return m_chip.x1Border(); }
    inline int x2Border() const { return m_chip.x2Border(); }
    inline int y1() const { return m_chip.y1(); }
    inline int y2() const { return m_chip.y2(); }
    inline int y1Total() const { return m_chip.y1Total(); }
    inline int y2Total() const { return m_chip.y2Total(); }
    inline int y1Border() const { return m_chip.y1Border(); }
    inline int y2Border() const { return m_chip.y2Border(); }

    inline void id(const QString& id) { m_chip.id(id); }
    inline void barcodes(int barcodes) { m_chip.barcodes(barcodes); }
    inline void name(const QString& name) { m_chip.name(name); }
    inline void x1(int x1) { m_chip.x1(x1); }
    inline void x2(int x2) { m_chip.x2(x2); }
    inline void x1Total(int x1Total) { m_chip.x1Total(x1Total); }
    inline void x2Total(int x2Total) { m_chip.x2Total(x2Total); }
    inline void x1Border(int x1Border) { m_chip.x1Border(x1Border); } 
    inline void x2Border(int x2Border) { m_chip.x2Border(x2Border); } 
    inline void y1(int y1) { m_chip.y1(y1); }
    inline void y2(int y2) { m_chip.y2(y2); }
    inline void y1Total(int y1Total) { m_chip.y1Total(y1Total); }
    inline void y2Total(int y2Total) { m_chip.y2Total(y2Total); }
    inline void y1Border(int y1Border){ m_chip.y1Border(y1Border); }
    inline void y2Border(int y2Border){ m_chip.y2Border(y2Border); }

    // get parsed data model
    const Chip& chip() const { return m_chip; }
    Chip& chip() { return m_chip; }

private:
    
    Q_DISABLE_COPY(ChipDTO)

    Chip m_chip;
};

#endif // CHIPDTO_H //
