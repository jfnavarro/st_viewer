#include "QuadTreeAABB.h"

QuadTreeAABB::QuadTreeAABB()
    : x(0.0)
    , y(0.0)
    , width(0.0)
    , height(0.0)
{
}

QuadTreeAABB::~QuadTreeAABB()
{
}

QuadTreeAABB::QuadTreeAABB(const qreal x, const qreal y, const qreal width, const qreal height)
    : x(x)
    , y(y)
    , width(width)
    , height(height)
{
}

QuadTreeAABB::QuadTreeAABB(const QPointF& p, const QSizeF& size)
    : x(p.x())
    , y(p.y())
    , width(size.width())
    , height(size.height())
{
}

QuadTreeAABB::QuadTreeAABB(const QRectF& rect)
    : x(rect.x())
    , y(rect.y())
    , width(rect.width())
    , height(rect.height())
{
}

const QuadTreeAABB QuadTreeAABB::fromPoints(const QPointF& p0, const QPointF& p1)
{
    return QuadTreeAABB(std::min(p0.x(), p1.x()),
                        std::min(p0.y(), p1.y()),
                        std::fabs(p1.x() - p0.x()),
                        std::fabs(p1.y() - p0.y()));
}

const QRectF QuadTreeAABB::toRectangle(const QuadTreeAABB& b)
{
    const qreal hW = 0.5 * b.width;
    const qreal hH = 0.5 * b.height;
    return QRectF(b.x + hW, b.y + hH, b.width, b.height);
}

const QuadTreeAABB QuadTreeAABB::split(SplitHalf split) const
{
    const qreal fW = width;
    const qreal hW = 0.5 * width;
    const qreal fH = height;
    const qreal hH = 0.5 * height;

    switch (split) {
    case H0:
        return QuadTreeAABB(x + 0.0, y + 0.0, fW, hH);
        break;
    case H1:
        return QuadTreeAABB(x + 0.0, y + hH, fW, hH);
        break;
    case V0:
        return QuadTreeAABB(x + 0.0, y + 0.0, hW, fH);
        break;
    case V1:
        return QuadTreeAABB(x + hW, y + 0.0, hW, fH);
        break;
    }

    return QuadTreeAABB();
}

const QuadTreeAABB QuadTreeAABB::split(SplitQuad split) const
{
    const qreal hW = 0.5 * width;
    const qreal hH = 0.5 * height;

    switch (split) {
    case Q0:
        return QuadTreeAABB(x + hW, y + hH, hW, hH);
        break;
    case Q1:
        return QuadTreeAABB(x + 0.0, y + hH, hW, hH);
        break;
    case Q2:
        return QuadTreeAABB(x + 0.0, y + 0.0, hW, hH);
        break;
    case Q3:
        return QuadTreeAABB(x + hW, y + 0.0, hW, hH);
        break;
    }

    return QuadTreeAABB();
}

const QPointF QuadTreeAABB::position() const
{
    return QPointF(x, y);
}

const QPointF QuadTreeAABB::middle() const
{
    return QPointF(x + (0.5 * width), y + (0.5 * height));
}

const QPointF QuadTreeAABB::end() const
{
    return QPointF(x + width, y + height);
}

const QPointF QuadTreeAABB::size() const
{
    return QPointF(width, height);
}

bool QuadTreeAABB::contains(const QPointF& p) const
{
    return (p.x() >= x && p.x() <= (x + width)) && (p.y() >= y && p.y() <= (y + height));
}

bool QuadTreeAABB::contains(const QuadTreeAABB& o) const
{
    return ((x <= o.x) && ((o.x + o.width) <= (x + width)) && (y <= o.y)
            && ((o.y + o.height) <= (y + height)));
}

bool QuadTreeAABB::intersects(const QuadTreeAABB& o) const
{
    // simple SAT (Separating Axis Theorem) approach
    return !((x >= (o.x + o.width)) || (y >= (o.y + o.height)) || ((x + width) <= o.x)
             || ((y + height) <= o.y));
}

const QuadTreeAABB QuadTreeAABB::cut(const QuadTreeAABB& o) const
{
    if (intersects(o)) {
        const QPointF p0 = STMath::max(position(), o.position());
        const QPointF p1 = STMath::min(end(), o.end());
        return QuadTreeAABB::fromPoints(p0, p1);
    } else {
        return QuadTreeAABB(0.0, 0.0, 0.0, 0.0);
    }
}

const QuadTreeAABB QuadTreeAABB::join(const QuadTreeAABB& o) const
{
    const QPointF p0 = STMath::min(position(), o.position());
    const QPointF p1 = STMath::max(end(), o.end());
    return QuadTreeAABB::fromPoints(p0, p1);
}

bool fuzzyEqual(const QuadTreeAABB& b0, const QuadTreeAABB& b1)
{
    return qFuzzyCompare(b0.x, b1.x) && qFuzzyCompare(b0.y, b1.y)
           && qFuzzyCompare(b0.width, b1.width) && qFuzzyCompare(b0.height, b1.height);
}

bool fuzzyNotEqual(const QuadTreeAABB& b0, const QuadTreeAABB& b1)
{
    return !qFuzzyCompare(b0.x, b1.x) || !qFuzzyCompare(b0.y, b1.y)
           || !qFuzzyCompare(b0.width, b1.width) || !qFuzzyCompare(b0.height, b1.height);
}

bool operator==(const QuadTreeAABB& b0, const QuadTreeAABB& b1)
{
    return (b0.x == b1.x) && (b0.y == b1.y) && (b0.width == b1.width) && (b0.height == b1.height);
}

bool operator!=(const QuadTreeAABB& b0, const QuadTreeAABB& b1)
{
    return (b0.x != b1.x) || (b0.y != b1.y) || (b0.width != b1.width) || (b0.height != b1.height);
}
