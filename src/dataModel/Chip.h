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
    explicit Chip(const Chip &other);
    ~Chip();

    Chip &operator=(const Chip &other);
    bool operator==(const Chip &other) const;

    // ID corresponds to the database ID of the object
    const QString id() const;
    // how many spots does the chip has
    unsigned spots() const;
    // the name of the chip
    const QString name() const;
    unsigned x1() const;
    unsigned x2() const;
    unsigned x1Total() const;
    unsigned x2Total() const;
    unsigned x1Border() const;
    unsigned x2Border() const;
    unsigned y1() const;
    unsigned y2() const;
    unsigned y1Total() const;
    unsigned y2Total() const;
    unsigned y1Border() const;
    unsigned y2Border() const;
    const QString created() const;
    const QString lastModified() const;

    void id(const QString &id);
    void spots(unsigned spots);
    void name(const QString &name);
    void x1(unsigned x1);
    void x2(unsigned x2);
    void x1Total(unsigned x1Total);
    void x2Total(unsigned x2Total);
    void x1Border(unsigned x1Border);
    void x2Border(unsigned x2Border);
    void y1(unsigned y1);
    void y2(unsigned y2);
    void y1Total(unsigned y1Total);
    void y2Total(unsigned y2Total);
    void y1Border(unsigned y1Border);
    void y2Border(unsigned y2Border);
    void created(const QString &created);
    void lastModified(const QString &lastModified);

private:
    QString m_id;
    unsigned m_spots;
    QString m_name;
    unsigned m_x1;
    unsigned m_x2;
    unsigned m_x1Total;
    unsigned m_x2Total;
    unsigned m_x1Border;
    unsigned m_x2Border;
    unsigned m_y1;
    unsigned m_y2;
    unsigned m_y1Total;
    unsigned m_y2Total;
    unsigned m_y1Border;
    unsigned m_y2Border;
    QString m_created;
    QString m_lastMofidied;
};

#endif // CHIPID_H
