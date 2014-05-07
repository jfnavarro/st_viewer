/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef CHIPID_H
#define CHIPID_H

#include <QString>

// Data model class to store chip data. Represents the on-board chip used to
// capture gene data. Contains various info and three distinct geometrical
// measures:
//     * Total bounds (aka. xxTotal)
//     * Border bounds (aka. xxBorder)
//     * Inner bounds (aka xx)
// Each touple contains absolute Cartesian coordinates and are laid out
// accordingly:
// (x1Total,y1Total)
//   ┌──────────────────────────┐
//   │ (x1Border,y1Border)      │
//   │  ┌─────────────────────┐ │
//   │  │(x1,y1)              │ │
//   │  │ ┌─────────────────┐ │ │
//   │  │ │                 │ │ │
//   │  │ │                 │ │ │
//   │  │ │                 │ │ │
//   │  │ │                 │ │ │
//   │  │ └─────────────────┘ │ │
//   │  │              (x2,y2)│ │
//   │  └─────────────────────┘ │
//   │       (x2Border,y2Border)│
//   └──────────────────────────┘
//                (x2Total,y2Total)
class Chip
{

public:

    Chip();
    explicit Chip(const Chip& other);
    ~Chip();

    Chip& operator=(const Chip& other);
    bool operator==(const Chip& other) const;

    const QString& id() const { return m_id; }
    int barcodes() const { return m_barcodes; }
    const QString& name() const { return m_name; }
    int x1() const { return m_x1; }
    int x2() const { return m_x2; }
    int x1Total() const { return m_x1Total; }
    int x2Total() const { return m_x2Total; }
    int x1Border() const { return m_x1Border; }
    int x2Border() const { return m_x2Border; }
    int y1() const { return m_y1; }
    int y2() const { return m_y2; }
    int y1Total() const { return m_y1Total; }
    int y2Total() const { return m_y2Total; }
    int y1Border() const { return m_y1Border; }
    int y2Border() const { return m_y2Border; }

    void id(const QString& id) { m_id = id; }
    void barcodes(int barcodes) { m_barcodes = barcodes; }
    void name(const QString& name) { m_name = name; }
    void x1(int x1) { m_x1 = x1;  }
    void x2(int x2) { m_x2 = x2; }
    void x1Total(int x1Total) { m_x1Total  = x1Total; }
    void x2Total(int x2Total) { m_x2Total  = x2Total; }
    void x1Border(int x1Border) { m_x1Border = x1Border; }
    void x2Border(int x2Border) { m_x2Border = x2Border; }
    void y1(int y1) { m_y1 = y1; }
    void y2(int y2)  { m_y2 = y2; }
    void y1Total(int y1Total) { m_y1Total  = y1Total; }
    void y2Total(int y2Total) { m_y2Total  = y2Total; }
    void y1Border(int y1Border) { m_y1Border = y1Border; }
    void y2Border(int y2Border) { m_y2Border = y2Border; }

private:

    QString m_id;
    int m_barcodes;
    QString m_name;
    int m_x1;
    int m_x2;
    int m_x1Total;
    int m_x2Total;
    int m_x1Border;
    int m_x2Border;
    int m_y1;
    int m_y2;
    int m_y1Total;
    int m_y2Total;
    int m_y1Border;
    int m_y2Border;
};

#endif // CHIPID_H
