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

    // ID corresponds to the database ID of the object
    const QString id() const;
    int barcodes() const;
    const QString name() const;
    int x1() const;
    int x2() const;
    int x1Total() const;
    int x2Total() const;
    int x1Border() const;
    int x2Border() const;
    int y1() const;
    int y2() const;
    int y1Total() const;
    int y2Total() const;
    int y1Border() const;
    int y2Border() const;
    const QString created() const;
    const QString lastModified() const;

    void id(const QString& id);
    void barcodes(int barcodes);
    void name(const QString& name);
    void x1(int x1);
    void x2(int x2);
    void x1Total(int x1Total);
    void x2Total(int x2Total);
    void x1Border(int x1Border);
    void x2Border(int x2Border);
    void y1(int y1);
    void y2(int y2);
    void y1Total(int y1Total);
    void y2Total(int y2Total);
    void y1Border(int y1Border);
    void y2Border(int y2Border);
    void created(const QString& created);
    void lastModified(const QString& lastModified);

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
    QString m_created;
    QString m_lastMofidied;
};

#endif // CHIPID_H
