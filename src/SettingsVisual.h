#ifndef SETTINGSVISUAL_H
#define SETTINGSVISUAL_H

#include <QString>
#include <QColor>

// Some settings and common variables for the main user windows
namespace Visual
{

/** GRAPHIC PARAMETERS DEFAULT VALUES **/

enum GeneColorMode {
    LinearColor = 1,
    LogColor = 2,
    ExpColor = 3,
};

// if user want to visualize read counts or number of genes or TPM read counts
enum GenePooledMode {
    PoolReadsCount = 1,
    PoolNumberGenes = 2,
    PoolTPMs = 3
};

enum Anchor {
    Center = 1,
    North = 2,
    NorthEast = 3,
    East = 4,
    SouthEast = 5,
    South = 6,
    SouthWest = 7,
    West = 8,
    NorthWest = 9,
    None = 10
};

// default color of gene is used in many places
static const QColor DEFAULT_COLOR_GENE = QColor::fromHsv(0, 255, 255, 255);

/** GRAPHICAL ITEMS ANCHOR POSITIONS **/
static const Anchor DEFAULT_ANCHOR_MINIMAP = SouthEast;
static const Anchor DEFAULT_ANCHOR_LEGEND = NorthEast;
static const Anchor DEFAULT_ANCHOR_GRID = None;
static const Anchor DEFAULT_ANCHOR_IMAGE = None;
static const Anchor DEFAULT_ANCHOR_GENE = None;
}
#endif // SETTINGSVISUAL_H
